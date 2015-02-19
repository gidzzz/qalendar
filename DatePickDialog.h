#ifndef DATEPICKDIALOG_H
#define DATEPICKDIALOG_H

#include "DateTimePickDialog.h"

#include <QListWidget>

#include <QDate>

class DatePickDialog : public DateTimePickDialog
{
    Q_OBJECT

    using DateTimePickDialog::centerView;

public:
    enum Type
    {
        Day,
        Week,
        Month,
        Year,
        TypeCount
    };

    DatePickDialog(Type type, QDate date, QWidget *parent);

    QDate date();

signals:
    void selected(QDate date);

private:
    Type type;

    QListWidget *dList;
    QListWidget *wList;
    QListWidget *mList;
    QListWidget *yList;

    int day();
    int month();
    int year();

private slots:
    void centerView();
    void adjustDays();
    void adjustWeeks();

    void accept();
};

#endif // DATEPICKDIALOG_H
