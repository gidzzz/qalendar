#include "TimePickSelector.h"

#include "TimePickDialog.h"

TimePickSelector::TimePickSelector(QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
    setCurrentTime(QTime::currentTime());
}

QTime TimePickSelector::currentTime() const
{
    return time;
}

void TimePickSelector::setCurrentTime(const QTime &time)
{
    // Make sure that the time does not contain the seconds part
    QTime roundTime = time.addSecs(-time.second());

    if (this->time != roundTime) {
        this->time = roundTime;
        emit selected(currentValueText());
    }
}

QString TimePickSelector::currentValueText() const
{
    return time.toString("hh:mm");
}

QWidget* TimePickSelector::widget(QWidget *parent)
{
    TimePickDialog *dialog = new TimePickDialog(time, parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog, SIGNAL(selected(QTime)), this, SLOT(onSelected(QTime)));

    return dialog;
}

void TimePickSelector::onSelected(QTime time)
{
    setCurrentTime(time);
}
