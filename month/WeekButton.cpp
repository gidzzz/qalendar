#include "WeekButton.h"

#include <QPixmap>
#include <QPainter>

#include "Date.h"

#include "Theme.h"
#include "MonthWidget.h"

const int Width = 74;

WeekButton::WeekButton(QWidget *parent) :
    QAbstractButton(parent)
{
    // Set font size
    QFont font = this->font();
    font.setPointSize(13);
    this->setFont(font);

    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

QSize WeekButton::sizeHint() const
{
    return QSize(Width, Metrics::MonthWidget::CellHeight);
}

void WeekButton::setDate(QDate date)
{
    this->date = date;

    this->setText(QString(tr("w%1")).arg(QString::number(Date::relWeekNumber(date)), 2, '0'));
}

void WeekButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, this->isDown() ? QPixmap(PIX_MONTH_WEEK_PRESSED) : QPixmap(PIX_MONTH_WEEK));

    painter.drawText(0, 0, this->width(), this->height(), Qt::AlignHCenter|Qt::AlignVCenter, this->text());
}

void WeekButton::onClicked()
{
    emit clicked(date);
}
