#ifndef CALENDARPICKDIALOG_H
#define CALENDARPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_CalendarPickDialog.h"

class CalendarPickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    CalendarPickDialog(QWidget *parent, int currentId);
    ~CalendarPickDialog();

signals:
    void selected(int id);

private:
    Ui::CalendarPickDialog *ui;

    void resizeEvent(QResizeEvent *e);

private slots:
    void onCalendarActivated(QListWidgetItem *item);
};

#endif // CALENDARPICKDIALOG_H
