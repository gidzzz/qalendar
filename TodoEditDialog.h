#ifndef TODOEDITDIALOG_H
#define TODOEDITDIALOG_H

#include "ComponentEditDialog.h"
#include "ui_TodoEditDialog.h"

#include <CTodo.h>

class TodoEditDialog : public ComponentEditDialog
{
    Q_OBJECT

public:
    TodoEditDialog(QWidget *parent, CTodo *todo = NULL);
    ~TodoEditDialog();

private:
    Ui::TodoEditDialog *ui;

    CTodo *todo;

    time_t alarmOffset;

private slots:
    void onDateChanged();
    void onAlarmChanged();

    void saveTodo();
    void deleteTodo();
};

#endif // TODOEDITDIALOG_H
