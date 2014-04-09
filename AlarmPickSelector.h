#ifndef ALARMPICKSELECTOR_H
#define ALARMPICKSELECTOR_H

#include <QDateTime>
#include <QMaemo5AbstractPickSelector>

#include <CAlarm.h>
#include <CComponent.h>

class AlarmPickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    AlarmPickSelector(int type, QObject *parent = 0);

    QString currentValueText() const;
    int currentSecondsBefore() const;

    QWidget* widget(QWidget *parent);

    void setReferenceDate(const QDateTime &date);
    void setSecondsBefore(int seconds);
    void setTrigger(int stamp);
    void setAlarm(CAlarm *alarm);

    void configureAlarm(CComponent *component) const;

    static QString textForAlarm(CAlarm &alarm);
    static QString textForAlarm(int type, int time);

private:
    bool enabled;
    int type;

    int secondsBefore;

    time_t referenceStamp;
    time_t offset;

    time_t triggerStamp() const;

private slots:
    void onSelected(bool enabled, int type, int beforeTime, int triggerTime);
};

#endif // ALARMPICKSELECTOR_H
