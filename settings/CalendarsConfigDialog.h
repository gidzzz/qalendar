#ifndef CALENDARSCONFIGDIALOG_H
#define CALENDARSCONFIGDIALOG_H

#include "RotatingDialog.h"
#include "ui_CalendarsConfigDialog.h"

namespace Ui {
    class CalendarsConfigDialog;
}

class CalendarsConfigDialog : public RotatingDialog
{
    Q_OBJECT

public:
    CalendarsConfigDialog(QWidget *parent);
    ~CalendarsConfigDialog();

private:
    Ui::CalendarsConfigDialog *ui;

    void reload();

private slots:
    void onCalendarActivated(QListWidgetItem *item);
    void onCalendarChanged(QListWidgetItem *item);

    void newCalendar();
};

#endif // CALENDARSCONFIGDIALOG_H
