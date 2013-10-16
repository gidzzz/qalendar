#ifndef TODOWINDOW_H
#define TODOWINDOW_H

#include "ui_TodoWindow.h"

#include <CTodo.h>

#include "ChangeClient.h"

class TodoWindow : public QMainWindow, public ChangeClient
{
    Q_OBJECT

public:
    TodoWindow(CTodo *todo, QWidget *parent);
    ~TodoWindow();

    void onChange();

private:
    Ui::TodoWindow *ui;

    CTodo *todo;

    void reload();

    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);

private slots:
    void editTodo();
    void cloneTodo();
    void deleteTodo();

    void enableLinks(bool enable);
};

#endif // TODOWINDOW_H
