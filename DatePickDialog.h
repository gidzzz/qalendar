#ifndef DATEPICKDIALOG_H
#define DATEPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_DatePickDialog.h"

#include <QListWidget>

#include <QDate>

namespace Ui {
    class DatePickDialog;
}

class DatePickDialog : public RotatingDialog
{
    Q_OBJECT

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
    Ui::DatePickDialog *ui;

    Type type;

    QListWidget *lists[TypeCount];

    int resizeCount;
    void resizeEvent(QResizeEvent *e);

    int row(Type type);

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
