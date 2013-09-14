#ifndef TEMPORALPLUG_H
#define TEMPORALPLUG_H

#include "Plug.h"

class TemporalPlug : public Plug
{
    Q_OBJECT

protected:
    TemporalPlug(QWidget *parent) : Plug(parent) { }

    // Convert the given date to a date that can be presented to the outside
    // world as the single date shown by this plug.
    virtual QDate toGlobalDate(QDate date) { return date; }

    // Sanitize the given date to a date expected by this plug internally, for
    // example if the year number is the only thing that matters, other parts
    // of the date can be erased to make comparisons straightforward.
    virtual QDate fromGlobalDate(QDate globalDate) { return globalDate; }

    // Process and share a date
    void setGlobalDate(QDate date)
    {
        this->date = toGlobalDate(date);
    }

    // Read and interpret the shared date
    QDate globalDate()
    {
        return fromGlobalDate(this->date);
    }

private:
    static QDate date;
};

#endif // TEMPORALPLUG_H
