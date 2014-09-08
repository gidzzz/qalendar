#ifndef TIMEPICKSELECTOR_H
#define TIMEPICKSELECTOR_H

#include <QMaemo5TimePickSelector>

class TimePickSelector : public QMaemo5TimePickSelector
{
    Q_OBJECT

public:
    QTime currentTime() const;
};

#endif // TIMEPICKSELECTOR_H
