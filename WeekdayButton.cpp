#include "WeekdayButton.h"

#include <QPixmap>
#include <QPainter>
#include <QMaemo5Style>

#include "DayWindow.h"

#include "Theme.h"

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

    // TODO: Make sure that the highlighted (current) day is in sync with the week widget

    const int margin = 3;

    // Draw the day name
    painter.drawText(0, margin, this->width(), this->height(), Qt::AlignHCenter|Qt::AlignTop, date.toString("ddd"));

    // Set font size for the date
    QFont font = painter.font();
    font.setPointSize(13);
    painter.setFont(font);

    if (!currentDay)
        painter.setPen(QMaemo5Style::standardColor("SecondaryTextColor"));

    // Draw the date
    painter.drawText(0, 0, this->width(), this->height()-margin, Qt::AlignHCenter|Qt::AlignBottom, date.toString("dd/MM"));
}

void WeekdayButton::onClicked()
{
    (new DayWindow(date, this))->show();
}
