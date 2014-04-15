#ifndef TODOSPLUG_H
#define TODOSPLUG_H

#include "Plug.h"

#include "ui_TodosPlug.h"

#include "ChangeManager.h"

class TodosPlug : public Plug
{
    Q_OBJECT

public:
    TodosPlug(QWidget *parent);
    ~TodosPlug();

    QString title() const;
    bool isRotatable() const { return true; }

    void onChange();

private:
    Ui::TodosPlug *ui;

    int numOverdue;
    int numUndone;
    int numTotal;

    void reload();

private slots:
    void onTodoActivated(QListWidgetItem *item = 0);
    void onTodoChanged(QListWidgetItem *item);

    void onContextMenuRequested(const QPoint &pos);
    void editCurrentTodo();
    void cloneCurrentTodo();
    void deleteCurrentTodo();

    void hideDoneTodos(bool hide);
    void cleanTodos();
};

#endif // TODOSPLUG_H
