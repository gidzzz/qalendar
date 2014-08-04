#include "AlarmPickSelector.h"

#include <limits>

#include "AlarmPickDialog.h"

#include "Date.h"

AlarmPickSelector::AlarmPickSelector(int type, QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
    this->type = type == E_AM_EXACTDATETIME ? E_AM_EXACTDATETIME : E_AM_ETIME;

    enabled = false;

    // Before-mode defaults
    secondsBefore = 0;

    // Trigger-mode defaults
    referenceStamp = 0;
    offset = -12 * 60*60;
}

void AlarmPickSelector::setSecondsBefore(int seconds)
{
    if (seconds < 0) {
        enabled = false;
    } else {
        enabled = true;
        type = E_AM_ETIME;
        secondsBefore = seconds;
    }

    emit selected(currentValueText());
}

void AlarmPickSelector::setTrigger(int stamp)
{
    if (stamp < 0) {
        enabled = false;
    } else {
        enabled = true;
        type = E_AM_EXACTDATETIME;

        offset = stamp - referenceStamp;
    }

    emit selected(currentValueText());
}

void AlarmPickSelector::setReferenceDate(const QDateTime &date)
{
    referenceStamp = date.toTime_t();

    emit selected(currentValueText());
}

void AlarmPickSelector::setAlarm(CAlarm *alarm, const QString &zone)
{
    if (alarm) {
        if (alarm->getDuration() == E_AM_EXACTDATETIME) {
            setTrigger(Date::toRemote(alarm->getTrigger(), zone).toTime_t());
        } else {
            setSecondsBefore(alarm->getTimeBefore());
        }
    } else {
        enabled = false;
    }
}

// There is no currentAlarm(), because its result would be impossible to forward
// to CComponent::setAlarm(CAlarm*) due to some obsolete hack in that function
// overriding the type of todo alarms.
void AlarmPickSelector::configureAlarm(CComponent *component, const QString &zone) const
{
    if (enabled) {
        if (type == E_AM_EXACTDATETIME) {
            // Convert the trigger to the desired time zone
            time_t triggerStamp = Date::toUtc(QDateTime::fromTime_t(this->triggerStamp()), zone);

            // WARNING: Setting the alarm to a moment in the past can lead to a segfault
            if (triggerStamp > (time_t) QDateTime::currentDateTime().toTime_t()) {
                component->setAlarm(triggerStamp, E_AM_EXACTDATETIME);
                return;
            }
        } else {
            component->setAlarm(secondsBefore, E_AM_ETIME);
            return;
        }
    }

    component->removeAlarm();
}

QString AlarmPickSelector::currentValueText() const
{
    return textForAlarm(type, enabled
                              ? type == E_AM_EXACTDATETIME ? triggerStamp() : secondsBefore
                              : -1);
}

int AlarmPickSelector::currentSecondsBefore() const
{
    return enabled && type == E_AM_ETIME ? secondsBefore : -1;
}

QWidget* AlarmPickSelector::widget(QWidget *parent)
{
    AlarmPickDialog *dialog = new AlarmPickDialog(type, secondsBefore, triggerStamp(), parent);

    connect(dialog, SIGNAL(selected(bool,int,int,int)), this, SLOT(onSelected(bool,int,int,int)));

    return dialog;
}

time_t AlarmPickSelector::triggerStamp() const
{
    // Calculate the new alarm trigger value in an under/overflow-proof way
    // NOTE: toTime_t() is a deceptive name, the return type is uint`
    if (offset > 0) {
        return referenceStamp > std::numeric_limits<time_t>::max() - offset
               ? std::numeric_limits<time_t>::max()
               : referenceStamp + offset;
    } else {
        return referenceStamp < -offset
               ? 0
               : referenceStamp + offset;
    }
}

void AlarmPickSelector::onSelected(bool enabled, int type, int beforeTime, int triggerTime)
{
    setSecondsBefore(beforeTime);
    setTrigger(triggerTime);

    this->type = type;
    this->enabled = enabled;

    emit selected(currentValueText());
}

QString AlarmPickSelector::textForAlarm(CAlarm &alarm)
{
    const int type = alarm.getDuration();
    const int time = type == E_AM_EXACTDATETIME ? alarm.getTrigger() : alarm.getTimeBefore();

    return textForAlarm(type, time);
}

QString AlarmPickSelector::textForAlarm(int type, int time)
{
    if (time < 0)
        return tr("None");

    if (type == E_AM_EXACTDATETIME)
        return Date::toString(QDateTime::fromTime_t(time), Date::Full, true);

    const int h = time / 3600;
    const int m = time % 3600 / 60;
    const int s = time % 60;

    const int numFieldsSet = (bool) h + (bool) m + (bool) s;

    if (numFieldsSet == 0) {
        return tr("Exact");
    } else if (numFieldsSet == 1) {
        return h ? tr("%n hour(s) before", "", h) :
               m ? tr("%n minute(s) before", "", m) :
                   tr("%n second(s) before", "", s);
    } else {
        return QString(tr("%1:%2:%3 before"))
               .arg(QString::number(h), 2, '0')
               .arg(QString::number(m), 2, '0')
               .arg(QString::number(s), 2, '0');
    }
}
