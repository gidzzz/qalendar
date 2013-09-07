#include "WeekWidget.h"

#include <QPainter>
#include <QDate>
#include <QMaemo5Style>

#include <QAbstractKineticScroller>
#include <QScrollArea>
#include <QScrollBar>

#include <CMulticalendar.h>
#include "CWrapper.h"

#include "Painter.h"
#include "Theme.h"

#include "ComponentWidget.h"

#include "WeekLayoutWindow.h"

#include "ChangeManager.h"

// TODO: Refresh the time indicator from time to time

using namespace Metrics::WeekWidget;
using namespace Metrics::WeekProfile;
using namespace Metrics::ComponentWidget;

const int NumHours = 24;
const int CellWidth  = 101;
const int CellHeight = 68;
const int SpacingHeight = 2;
const int SpacingWidth  = 3;

WeekWidget::WeekWidget(QWidget *parent) :
    QWidget(parent),
    allDaySlots(1)
{
}

WeekWidget::~WeekWidget()
{
    cleanup();
}

QSize WeekWidget::sizeHint() const
{
    return QSize(TimeWidth + (CellWidth + SpacingWidth) * NumWeekdays, (CellHeight + SpacingHeight) * NumHours + allDayRowHeight() + SpacingHeight);
}

void WeekWidget::setDate(QDate date)
{
    this->date = date;
    reload();
}

void WeekWidget::cleanup()
{
    for (unsigned int i = 0; i < instances.size(); i++)
        delete instances[i];
    instances.clear();

    for (unsigned int i = 0; i < components.size(); i++)
        delete components[i];
    components.clear();

    for (unsigned int i = 0; i < allDayComponents.size(); i++)
        delete allDayComponents[i];
    allDayComponents.clear();

    for (unsigned int i = 0; i < allDayInstances.size(); i++)
        delete allDayInstances[i];
    allDayInstances.clear();
}

// Refresh the list of events and repopulate the widget
void WeekWidget::reload()
{
    // Clear collections
    palette.clear();
    weekProfile.clear();
    hintProfile.clear();

    // Delete all component widgets
    QList<ComponentWidget*> widgets = this->findChildren<ComponentWidget*>();
    for (int i = 0; i < widgets.size(); i++)
        widgets.at(i)->deleteLater();

    // Free allocated resources
    cleanup();

    // Determine the first and last visible timestamp
    const time_t startStamp = QDateTime(WeekWidget::firstDate()).toTime_t();
    const time_t   endStamp = QDateTime(WeekWidget::lastDate() ).toTime_t() - 1;

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();

    int offset = 0;
    int error;

    for (unsigned int i = 0; i < calendars.size(); i++) {
        CCalendar *calendar = calendars[i];

        if (!calendar->IsShown()) continue;

        vector<CComponent*> componentsPart = calendar->getAllEventsTodos(startStamp, endStamp, 1024, offset, error);

        palette[calendar->getCalendarId()] = calendar->getCalendarColor();

        for (unsigned int c = 0; c < componentsPart.size(); c++) {
            if (componentsPart[c]->getAllDay()
            ||  componentsPart[c]->getType() != E_EVENT)
            {
                if (componentsPart[c]->getType() == E_TODO
                &&  static_cast<CTodo*>(componentsPart[c])->getStatus())
                {
                    delete componentsPart[c];
                } else {
                    allDayComponents.push_back(componentsPart[c]);
                }
            } else {
                components.push_back(componentsPart[c]);
            }
        }
    }

    mc->releaseListCalendars(calendars);

    // Generate instances for all-day components
    CWrapper::expand(allDayComponents, allDayInstances, startStamp, endStamp);
    CWrapper::sort(allDayInstances);

    // Add sorted component instances to the profile
    CWrapper::expand(components, instances, startStamp, endStamp);
    CWrapper::sort(instances);
    for (unsigned int i = 0; i < instances.size(); i++)
        weekProfile.add(instances[i]);

    populateAllDay();
    populate();

    // Create a hint profile from the accumulated widgets
    hintProfile.populate(componentWidgets);
    componentWidgets.clear();

    // The proper height can change depending on the number of all-day widgets
    this->setFixedHeight(sizeHint().height());

    this->update();
}

void WeekWidget::populateAllDay()
{
    allDaySlots = 1;

    if (allDayInstances.empty()) return;

    WeekLayoutWindow window;
    unsigned int currentComponent = 0;
    QDate date = firstDate();
    const QDate lastDate = this->lastDate();

    // Iterate over weekday columns
    for (int d = 0; d < NumWeekdays; d++) {
        // Move to a new day
        window.move(date);
        date = date.addDays(1);
        const time_t nextDayStamp = QDateTime(date).toTime_t();

        // Fill the window with new events
        while (currentComponent < allDayInstances.size() &&
               allDayInstances[currentComponent]->stamp < nextDayStamp)
        {
            window.add(allDayInstances[currentComponent++]);
        }

        // Create a widget for each new component
        for (unsigned int i = 0; i < window.instances.size(); i++) {
            if (window.instances[i] && window.initial[i]) {
                // Calculate how many days of the component are visible
                int componentLength;
                if (window.instances[i]->component->getType() == E_EVENT) {
                    QDateTime endDate = QDateTime::fromTime_t(window.instances[i]->end());
                    if (window.instances[i]->duration() > 0)
                        endDate = endDate.addSecs(-1);

                    componentLength = 1 + qMin(date.daysTo(endDate.date()) + 1,
                                               date.daysTo(lastDate));
                } else {
                    componentLength = 1;
                }

                const int eventX = TimeWidth + d * (CellWidth + SpacingWidth);
                const int eventY = i * AllDayHeight + (CellHeight - AllDayHeight) / 2;
                const int eventW = componentLength * (CellWidth + SpacingWidth) - SpacingWidth;

                const int color = palette[window.instances[i]->component->getCalendarId()];

                ComponentWidget *widget = new ComponentWidget(window.instances[i], color, eventX, eventY, eventW, AllDayHeight, this);
                componentWidgets.push_back(widget);
                widget->show();
            }
        }

        allDaySlots = qMax(allDaySlots, (int) window.instances.size());
    }
}

void WeekWidget::populate()
{
    if (instances.empty()) return;

    QHash<ComponentInstance*,ComponentWidget*> masterWidgets;
    masterWidgets.reserve(components.size());

    for (int d = 0; d < NumWeekdays; d++) {
        const int eventBaseX = TimeWidth + d * (CellWidth + SpacingWidth);
        const time_t dayStartStamp = QDateTime(firstDate().addDays(d)).toTime_t();
        const time_t   dayEndStamp = QDateTime(firstDate().addDays(d+1)).toTime_t() - 1;

        bool dayFirstFrame = true;

        // Analyze the profile frame by frame
        for (unsigned int f = 0; f < weekProfile.frames.size()-1; f++) {
            // A frame which is on the previous day can only be useful if it is
            // the first one to cover another day. This can be determined by
            // checking if its successor is on the second day.
            if (weekProfile.frames[f+1].stamp < dayStartStamp) continue;

            // Take a frame from which events will be built
            WeekProfile::Frame frame = weekProfile.frames[f];

            // End this pass if a frame from the next day is encountered
            if (frame.stamp > dayEndStamp) break;

            // Look at each component in the selected frame
            for (int i = 0; i < FrameSize; i++) {
                // Take a look at a slot, check if it is occupied and if a new widget should be created
                if (frame.instances[i] && (dayFirstFrame || frame.initial[i])) {
                    ComponentInstance *instance = frame.instances[i];

                    // Ignore components which do not reach this day
                    if (instance->end() < dayStartStamp
                    ||  instance->end() == dayStartStamp && instance->duration() > 0)
                        continue;

                    // Calculate the key moments of the component, relative to this day
                    const int eventStart  = qMax((time_t) 0, (instance->stamp - dayStartStamp));
                    const int eventLength = qMin((time_t) NumHours*60*60 - eventStart, instance->end() - qMax(dayStartStamp, instance->stamp));

                    // Initialize component size parameters to the worst case
                    int maxPofileWidth = 1;
                    int minProfileIndent = FrameSize;
                    int minFreeSlots = FrameSize;

                    // Look for opprtunities to take more than one slot with
                    // this component by peeking into future frames
                    for (unsigned int ff = f; ff < weekProfile.frames.size(); ff++) {
                        // There is only a need to look as far as this component reaches
                        if (weekProfile.frames[ff].instances[i] != frame.instances[i]) break;
                        // Width on each day can be different, look no further than this day
                        if (weekProfile.frames[ff].stamp > dayEndStamp) break;

                        // The greatest frame width will be the denominator for component width
                        maxPofileWidth = qMax(maxPofileWidth, weekProfile.frames[ff].width());

                        minProfileIndent = qMin(minProfileIndent, weekProfile.frames[ff].indent());

                        // Find the first occupied slot to the right
                        for (int ii = i+1; ii < FrameSize; ii++) {
                            if (weekProfile.frames[ff].instances[ii]) {
                                // The amount of minimum free space will be the component width numerator
                                const int freeSlots = ii - i;
                                if (freeSlots < minFreeSlots)
                                    minFreeSlots = freeSlots;
                                break;
                            }
                        }
                    }

                    // Make sure that the amount of free slots does not take unoccupied border slots into account
                    minFreeSlots = qMin(minFreeSlots, maxPofileWidth-i);

                    // Take the indent into account to get the real usable width
                    maxPofileWidth -= minProfileIndent;

                    const int eventX = eventBaseX + (i-minProfileIndent) * CellWidth / maxPofileWidth;
                    const int eventY = (allDayRowHeight() + SpacingHeight) + (CellHeight + SpacingHeight) * eventStart/60/60;
                    const int eventW = CellWidth * minFreeSlots / maxPofileWidth;
                    const int eventH = qMax(CellHeight / 2, (CellHeight + SpacingHeight) * eventLength/60/60 - SpacingHeight);

                    const int color = palette[instance->component->getCalendarId()];

                    ComponentWidget *widget = new ComponentWidget(instance, color, eventX, eventY, eventW, eventH, this);
                    componentWidgets.push_back(widget);
                    widget->show();

                    // Create a master-slave relationship if necessary
                    if (ComponentWidget *master = masterWidgets.value(instance)) {
                        master->addSlave(widget);
                    } else {
                        masterWidgets[instance] = widget;
                    }
                }
            }

            dayFirstFrame = false;
        }
    }
}

// Returns the first day shown by the widget
QDate WeekWidget::firstDate()
{
    // Align to the beginning of the week
    return date.addDays(-date.dayOfWeek() + 1);
}

// Returns the last day shown by the widget
QDate WeekWidget::lastDate()
{
    return firstDate().addDays(7);
}

int WeekWidget::allDayRowHeight() const
{
    return allDaySlots * AllDayHeight + (CellHeight - AllDayHeight);
}

void WeekWidget::renderRow(int hour, int baseY, QPainter &painter)
{
    const int cellH = hour < 0 ? allDayRowHeight() : CellHeight;

    //QPixmap pixHourLine(PIX_WEEK_HOUR_LINE); // TODO: Use this as the vertical spacer?
    QPixmap pixDayShade(PIX_WEEK_DAY_SHADE);
    QPixmap pixDayShadeCurrent(PIX_WEEK_DAY_SHADE_CURRENT);
    QPixmap pixDayPressed(PIX_GENERIC_PRESSED);

    // Draw current time indicator
    const QTime time = QTime::currentTime();
    if (time.hour() == hour) {
        painter.setPen(QMaemo5Style::standardColor("ActiveTextColor"));
        const int y = baseY + cellH * time.minute() / 60;
        painter.drawLine(0, y, TimeWidth, y);
    }

    // Set hour label font
    QFont font = painter.font();
    font.setPointSize(13);
    painter.setFont(font);
    painter.setPen(QMaemo5Style::standardColor("DefaultTextColor"));

    // Draw hour label
    painter.drawText(0, baseY + 3, TimeWidth, cellH,
                     Qt::AlignTop|Qt::AlignHCenter|Qt::TextWordWrap,
                     hour < 0 ? tr("All day") : QString("%1:00").arg(QString::number(hour), 2, '0'));

    const int currentDay = QDate::currentDate().dayOfWeek();
    const bool isCurrentWeek = firstDate() <= QDate::currentDate() && QDate::currentDate() < lastDate();

    // Draw day cells
    for (int x = TimeWidth, d = 1; d <= NumWeekdays; d++) {
        painter.drawPixmap(x, baseY, CellWidth, cellH,
                           isCurrentWeek && currentDay == d ? pixDayShadeCurrent : pixDayShade);

        if (pressedDate.isValid()
        &&  pressedDate.date().dayOfWeek() == d
        &&  (pressedAllDay && hour < 0 || !pressedAllDay && pressedDate.time().hour() == hour))
        {
            Painter::drawStretched(painter, pixDayPressed, QRect(x, baseY, CellWidth, cellH));
        }

        x += CellWidth + SpacingWidth;
    }
}

void WeekWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    const QRect rect = e->rect();
    const int adrH = allDayRowHeight() + SpacingHeight;

    // Draw the all-day row separately, if visible
    if (rect.top() < adrH)
        renderRow(-1, 0, painter);

    // Draw all visible hours
    for (int y = adrH, h = 0; h < NumHours; h++, y += CellHeight + SpacingHeight) {
        // Skip rows before the visible rect
        if (y + CellHeight + SpacingHeight < rect.top())
            continue;
        // Finish when the first row after the visible rect is encountered
        if (y > rect.bottom())
            break;

        renderRow(h, y, painter);
    }
}

void WeekWidget::mousePressEvent(QMouseEvent *e)
{
    pressedPoint = e->pos();
    pressedDate = mapToDate(e->pos());
    pressedAllDay = inAllDay(e->pos());

    this->update();
}

void WeekWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (pressedDate.isValid()
    &&  mapToDate(e->pos()) == pressedDate
    &&  pressedAllDay == inAllDay(e->pos()))
    {
        // Create a new event in the chosen time window
        ChangeManager::newEvent(this,
                                pressedDate,
                                pressedDate.addSecs(pressedAllDay ? 24*60*60-1 : 60*60),
                                pressedAllDay);
    } else {
        // Toggle view between current time and all-day row
        if (!pressedPoint.isNull() && this->parentWidget()) {
            if (QScrollArea *scrollArea = qobject_cast<QScrollArea*>(this->parentWidget()->parentWidget())) {
                QScrollBar *scrollBar = scrollArea->verticalScrollBar();
                const QTime currentTime = QTime::currentTime();
                const int scrollPosition = scrollBar->value();

                // Scroll position for the beginning of all-day row
                const int allDayPosition = 0;

                // Scroll position to have the time indicator in the middle
                const int currentTimePosition = allDayRowHeight() + SpacingHeight
                                            + currentTime.hour() * (CellHeight + SpacingHeight)
                                            + currentTime.minute() * CellHeight / 60
                                            - scrollArea->viewport()->height() / 2;

                // Scroll to the position which is further away
                scrollArea->property("kineticScroller").value<QAbstractKineticScroller*>()
                          ->scrollTo(QPoint(0, qBound(0, qAbs(scrollPosition-allDayPosition) > qAbs(scrollPosition-currentTimePosition)
                                                      ? allDayPosition
                                                      : currentTimePosition,
                                                      scrollBar->maximum())));
            }
        }
    }

    forgetPress();

    this->update();
}

void WeekWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (pressedPoint.isNull()) return;

    // This discards a fake move event when scrolling starts
    if (e->pos() == QPoint(-1000,-1000)) return;

    if (e->pos().x() - pressedPoint.x() > SwipeThold) {
        forgetPress();
        emit swipedPrev();
    } else if (e->pos().x() - pressedPoint.x() < -SwipeThold) {
        forgetPress();
        emit swipedNext();
    }
}

// Maps a coordinate in the widget space to the corresponding day and hour
QDateTime WeekWidget::mapToDate(const QPoint &pos)
{
    int x = (pos.x() - TimeWidth);
    int y = (pos.y() - allDayRowHeight() - SpacingHeight);

    if (x < 0) {
        return QDateTime();
    } else {
        x = x < 0 ? -1 : x / (CellWidth + SpacingWidth);
        y = y < 0 ? -1 : y / (CellHeight + SpacingHeight);

        return QDateTime(firstDate().addDays(x), QTime(y,00));
    }
}

// Checks if the given point is within the all-day area
bool WeekWidget::inAllDay(const QPoint &pos)
{
    return pos.y() < allDayRowHeight() + SpacingHeight;
}

void WeekWidget::forgetPress()
{
    pressedPoint = QPoint();
    pressedDate = QDateTime();
}
