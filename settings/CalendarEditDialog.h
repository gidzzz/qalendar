#ifndef CALENDAREDITDIALOG_H
#define CALENDAREDITDIALOG_H

#include "RotatingDialog.h"
#include "ui_CalendarEditDialog.h"

#include <QButtonGroup>

#include "CCalendar.h"

class CalendarEditDialog : public RotatingDialog
{
    Q_OBJECT

public:
    CalendarEditDialog(int calendarId, QWidget *parent);
    ~CalendarEditDialog();

    int calendarId();

    void setCalendarName(QString name);

private:
    Ui::CalendarEditDialog *ui;

    QButtonGroup *typeGroup;

    CCalendar *calendar;

private slots:
    void saveCalendar();
    void exportCalendar();
    void deleteCalendar();
};

#endif // CALENDAREDITDIALOG_H
