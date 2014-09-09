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

    void setDue(QDate due);
    void setCalendar(int calendarId);

private:
    Ui::TodoEditDialog *ui;

    CTodo *todo;

    bool saveDefaults;
    bool saveDue;

private slots:
    void onDateChanged();

    void saveTodo();
    void deleteTodo();
};

#endif // TODOEDITDIALOG_H
