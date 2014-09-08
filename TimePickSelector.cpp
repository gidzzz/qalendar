#include "TimePickSelector.h"

QTime TimePickSelector::currentTime() const
{
    // Make sure that the returned time does not contain the seconds part
    const QTime currentTime = QMaemo5TimePickSelector::currentTime();
    return currentTime.addSecs(-currentTime.second());
}
