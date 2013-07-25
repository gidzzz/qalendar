#ifndef WEEKPLUG_H
#define WEEKPLUG_H

#include "TemporalPlug.h"

#include <QHBoxLayout>

#include <QDate>

#include "WeekWidget.h"

class WeekPlug : public TemporalPlug
{
    Q_OBJECT

public:
    WeekPlug(QDate date, QWidget *parent);

    QString title() const;

    void onActivated();

    void setDate(QDate date);

private:
    QDate date;

    QHBoxLayout *weekdayLayout;
    WeekWidget *weekWidget;

    QDate toGlobalDate(QDate date);
    QDate fromGlobalDate(QDate globalDate);

private slots:
    void selectWeek();
    void gotoPrevWeek();
    void gotoNextWeek();
    void gotoToday();

    void newEvent();
};

#endif // WEEKPLUG_H
