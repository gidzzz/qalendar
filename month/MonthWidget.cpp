#include "MonthWidget.h"

#include <QPainter>
#include <QDate>
#include <QMaemo5Style>

#include <CMulticalendar.h>
#include "CWrapper.h"

#include "MonthLayoutWindow.h"

#include "DayWindow.h"

#include "Date.h"

#include "Theme.h"
#include "Metrics.h"

using namespace Metrics::MonthWidget;
using namespace Metrics::Pixmap;

const int EventHeight = 6;
const int SwipeThold = 70;

MonthWidget::MonthWidget(QDate date, QWidget *parent) :
    QWidget(parent),
    date(date)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (date.isValid())
        reload();
}

void MonthWidget::setDate(QDate date)
{
    this->date = date;
    reload();
}

// Refresh the list of events and redraw the the widget
void MonthWidget::reload()
{
    // Clean up
    for (unsigned int i = 0; i < instances.size(); i++)
        delete instances[i];
    for (unsigned int i = 0; i < components.size(); i++)
        delete components[i];
    instances.clear();
    components.clear();
    palette.clear();

    const time_t startStamp = QDateTime(MonthWidget::firstDate()).toTime_t();
    const time_t endStamp = QDateTime(MonthWidget::lastDate()).toTime_t()-1;

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();

    // Get components from each calendar
    for (unsigned int i = 0; i < calendars.size(); i++) {
        CCalendar *calendar = calendars[i];
        if (!calendar->IsShown()) continue;

        palette[calendar->getCalendarId()] = calendar->getCalendarColor();

        int offset = 0;
        int error;
        vector<CComponent*> componentsPart = calendar->getAllEventsTodos(startStamp, endStamp, 1024, offset, error);

        for (unsigned int c = 0; c < componentsPart.size(); c++) {
            if (componentsPart[c]->getType() == E_TODO
            && (static_cast<CTodo*>(componentsPart[c])->getStatus()))
            // TODO: Check if there are any invisible todos (from other days)
            {
                delete componentsPart[c];
            } else {
                components.push_back(componentsPart[c]);
            }
        }
    }

    mc->releaseListCalendars(calendars);

    CWrapper::expand(components, instances, startStamp, endStamp);
    CWrapper::sort(instances); // TODO: Only full day differences should matter here

    this->update();
}

// Returns the first day shown by the widget
QDate MonthWidget::firstDate()
{
    const int currentMonth = date.month();

    // Align to the beginning of the week
    QDate first = date.addDays(1 - Date::relDayOfWeek(date.dayOfWeek()));

    // Repeat until the last week of the previous month is reached
    while (first.month() == currentMonth)
        first = first.addDays(-7);

    return first;
}

// Returns the last day shown by the widget
QDate MonthWidget::lastDate()
{
    return firstDate().addDays(NumWeekdays * NumWeeks);
}

// Maps a coordinate in the widget space to the corresponding day
QDate MonthWidget::mapToDate(const QPoint &pos)
{
    return firstDate().addDays(pos.y()/CellHeight * NumWeekdays + pos.x()/CellWidth);
}

QPixmap MonthWidget::render()
{
    using namespace Metrics::MonthLayoutWindow;

    QPixmap canvas(CellWidth * NumWeekdays, CellHeight * NumWeeks);
    canvas.fill(Qt::transparent);
    QPainter painter(&canvas);

    // Load pixmaps
    QPixmap pixDayCurrent(PIX_MONTH_CURRENT);
    QPixmap pixDayDimmed(PIX_MONTH_DIMMED);
    QPixmap pixDay(PIX_MONTH);
    QPixmap pixDayPressed(PIX_MONTH_PRESSED);

    // Prepare colors
    QColor textColor = QMaemo5Style::standardColor("DefaultTextColor");
    QColor dimmedTextColor = QMaemo5Style::standardColor("SecondaryTextColor");
    QColor reversedTextColor = QMaemo5Style::standardColor("ReversedTextColor");

    // Prepare the font
    QFont font = painter.font();
    font.setPointSize(13);
    font.setBold(true);
    painter.setFont(font);

    // Calculate dates
    QDate date = this->date;
    const int currentMonth = date.month();
    date = firstDate();

    // Fill the event window with initial entries
    unsigned int currentComponent = 0;
    MonthLayoutWindow window;
    while (currentComponent < instances.size() &&
           instances[currentComponent]->stamp < (time_t) QDateTime(date).toTime_t())
    {
        if (instances[currentComponent]->component->getType() == E_EVENT)
            window.add(instances[currentComponent]);

        currentComponent++;
    }

    // Iterate over week rows
    for (int y = 0, w = 0; w < NumWeeks; w++) {
        // Iterate over weekday columns
        for (int x = 0, d = 0; d < NumWeekdays; d++) {

            const bool selectedMonth = date.month() == currentMonth;

            // Draw background
            if (date == pressedDate) {
                // Pressed
                painter.drawPixmap(x, y, pixDayPressed);
            } else if (selectedMonth) {
                // Current month
                if (date == QDate::currentDate()) {
                    // Today
                    painter.drawPixmap(x, y, pixDayCurrent);
                } else {
                    // Normal
                    painter.drawPixmap(x, y, pixDay);
                }
            } else {
                // Other month
                painter.drawPixmap(x, y, pixDayDimmed);
            }

            const int topMargin = 8;
            const int bottomMargin = 6;
            const int leftMargin = 8;
            const int rightMargin = 6;
            const int colSpacing = 2;
            const int baseWidth = (CellWidth - leftMargin - rightMargin) / 4;

            // Draw day number
            painter.setPen(selectedMonth ? (date == QDate::currentDate() ? reversedTextColor : textColor) : dimmedTextColor);
            painter.drawText(x+leftMargin,
                             y+topMargin,
                             CellWidth-leftMargin-rightMargin,
                             CellHeight-topMargin-bottomMargin,
                             Qt::AlignBottom | Qt::AlignRight,
                             QString::number(date.day()));

            // Move the window
            const time_t thisDayStamp = QDateTime(date).toTime_t();
            window.cleanup(date);
            date = date.addDays(1);
            const time_t nextDayStamp = QDateTime(date).toTime_t();

            bool todo = false;
            bool bday = false;

            // Fill the window
            while (currentComponent < instances.size() &&
                   instances[currentComponent]->stamp < nextDayStamp)
            {
                switch (instances[currentComponent]->component->getType()) {
                    case E_EVENT: window.add(instances[currentComponent]); break;
                    case E_TODO: todo = true; break;
                    case E_BDAY: bday = true; break;
                }

                currentComponent++;
            }

            // Draw event stripes
            for (int i = 0; i < WindowSize; i++) {
                for (int j = 0; j < WindowColumns; j++) {
                    if (window.instances[i][j]) {
                        // Determine the shape characteristics of the stripe
                        const bool extendsLeft = window.instances[i][j]->stamp < thisDayStamp;
                        const bool extendsRight = window.instances[i][j]->end() > nextDayStamp;
                        const bool doubleWidth = window.instances[i][0] == window.instances[i][1];

                        // Obtain stripe pixmap
                        QIcon eventIcon = QIcon::fromTheme(selectedMonth ?
                                                           CWrapper::colorStripe(palette[window.instances[i][j]->component->getCalendarId()]) :
                                                           CWrapper::colorStripeDim(palette[window.instances[i][j]->component->getCalendarId()]));
                        QPixmap eventPixmap = eventIcon.pixmap(eventIcon.availableSizes().first());

                        // Determine the horizontal coordinate of the stripe
                        int eventX = extendsLeft ? x : x+leftMargin;
                        if (extendsLeft) {
                            eventX = x;
                        } else if (j == 0) {
                            eventX = x + leftMargin;
                        } else {
                            eventX = x + leftMargin + baseWidth + colSpacing;
                        }

                        // Determine the width of the stripe
                        int eventW = extendsRight ? CellWidth : (CellWidth-leftMargin-rightMargin) / 2 + (extendsLeft ? leftMargin : 0);
                        if (extendsRight) {
                            eventW = CellWidth;
                        } else if (doubleWidth) {
                            eventW = baseWidth + colSpacing + baseWidth;
                        } else {
                           eventW = baseWidth;
                        }

                        // Draw the stripe
                        painter.drawPixmap(eventX,
                                           y + topMargin + i*EventHeight,
                                           eventW,
                                           EventHeight,
                                           eventPixmap,
                                           eventPixmap.width()/3,
                                           0,
                                           eventPixmap.width()/3,
                                           eventPixmap.height());

                        // Skip the second column if necessary
                        if (doubleWidth)
                            j++;
                    }
                }
            }

            const int emblemMargin = 6;

            if (todo)
                // Draw todo emblem
                painter.drawPixmap(x + CellWidth - emblemMargin - EmblemSize - (bday ? EmblemSize + emblemMargin + emblemMargin : 0),
                                   y + emblemMargin,
                                   *Theme::TodoEmblem);
            if (bday)
                // Draw birthday emblem
                painter.drawPixmap(x + CellWidth - emblemMargin - EmblemSize,
                                   y + emblemMargin,
                                   *Theme::BdayEmblem);

            x += CellWidth;
        }

        y += CellHeight;
    }

    return canvas;
}

void MonthWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, render());
}

void MonthWidget::mousePressEvent(QMouseEvent *e)
{
    pressedPoint = e->pos();
    pressedDate = mapToDate(e->pos());

    this->update();
}

void MonthWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (mapToDate(e->pos()) == pressedDate)
        (new DayWindow(mapToDate(e->pos()), this))->show();

    forgetPress();

    this->update();
}

void MonthWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (pressedPoint.isNull()) return;

    const int dx = e->pos().x() - pressedPoint.x();
    const int dy = e->pos().y() - pressedPoint.y();

    if (qAbs(dx) > qAbs(dy*2)) { // Horizontal motion
        if (dx > SwipeThold) {
            forgetPress();
            emit swipedPrevFar();
        } else if (dx < -SwipeThold) {
            forgetPress();
            emit swipedNextFar();
        }
    } else if (qAbs(dy) > qAbs(dx*2)){ // Vertical motion
        if (dy > SwipeThold) {
            forgetPress();
            emit swipedPrev();
        } else if (dy < -SwipeThold) {
            forgetPress();
            emit swipedNext();
        }
    }
}

void MonthWidget::forgetPress()
{
    pressedPoint = QPoint();
    pressedDate = QDate();
}
