#ifndef TEMPORALPLUG_H
#define TEMPORALPLUG_H

#include "Plug.h"

class TemporalPlug : public Plug
{
    Q_OBJECT

protected:
    TemporalPlug(QWidget *parent) : Plug(parent) { }

    virtual QDate toGlobalDate(QDate date) { return date; }
    virtual QDate fromGlobalDate(QDate globalDate) { return globalDate; }

    QDate globalDate() { return fromGlobalDate(this->date); }
    void setGlobalDate(QDate date) { this->date = toGlobalDate(date); }

private:
    static QDate date;
};

#endif // TEMPORALPLUG_H
