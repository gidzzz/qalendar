#include "WeekdayButton.h"

#include <QPixmap>
#include <QPainter>
#include <QMaemo5Style>

#include "DayWindow.h"

#include "Date.h"
#include "Theme.h"

// TODO: Make sure that the highlighted (current) day is in sync with the week
// widget? On the second thought, keeping them perfectly synchronized might be
// not worth the complications, as the automatic date-based refresh will kick in
// a few seconds after midnight anyway.

WeekdayButton::WeekdayButton(QWidget *parent) :
    QAbstractButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

QSize WeekdayButton::sizeHint() const
{
    return QSize(104, 56);
}

void WeekdayButton::setDate(QDate date)
{
    this->date = date;
    this->update();
}

void WeekdayButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    bool currentDay = date == QDate::currentDate();

    // Draw the background
    painter.drawPixmap(0, 0, QPixmap(this->isDown()
                                   ? PIX_WEEKDAY_PRESSED
                                   : (currentDay
                                    ? PIX_WEEKDAY_CURRENT
                                    : PIX_WEEKDAY)));

    if (currentDay)
        painter.setPen(QMaemo5Style::standardColor("ReversedTextColor"));

    const int margin = 3;

    // Draw the day name
    painter.drawText(0, margin, this->width(), this->height(),
                     Qt::AlignHCenter|Qt::AlignTop,
                     QLocale().standaloneDayName(date.dayOfWeek(), QLocale::ShortFormat));

    // Set font size for the date
    QFont font = painter.font();
    font.setPointSize(13);
    painter.setFont(font);

    if (!currentDay)
        painter.setPen(QMaemo5Style::standardColor("SecondaryTextColor"));

    // Draw the date
    painter.drawText(0, 0, this->width(), this->height()-margin,
                     Qt::AlignHCenter|Qt::AlignBottom,
                     Date::toString(date, Date::Partial));
}

void WeekdayButton::onClicked()
{
    (new DayWindow(date, this))->show();
}
