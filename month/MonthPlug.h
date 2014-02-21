#ifndef MONTHPLUG_H
#define MONTHPLUG_H

#include "TemporalPlug.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDate>

#include "MonthWidget.h"

class MonthPlug : public TemporalPlug
{
    Q_OBJECT

public:
    MonthPlug(QDate date, QWidget *parent);

    QString title() const;

    void onChange();

    void setDate(QDate date);

signals:
    void weekActivated(QDate date);

private:
    QDate date;

    MonthWidget *monthWidget;
    QVBoxLayout *weekLayout;
    QHBoxLayout *dayLayout;
    QPixmap background;

    void paintEvent(QPaintEvent *);

    QDate toGlobalDate(QDate date);
    QDate fromGlobalDate(QDate globalDate);

private slots:
    void selectMonth();
    void gotoPrevMonth();
    void gotoNextMonth();
    void gotoPrevYear();
    void gotoNextYear();
    void gotoToday();

    void newEvent();

    void onWeekClicked(QDate date);
};

#endif // MONTHPLUG_H
