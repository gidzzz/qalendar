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

    QSet<int> visibleGroups;

    QString statusString(int numOverdue, int numUndone, int numTotal) const;

    void reload();
    void populateSingle(vector<CCalendar*> &calendars, bool hideDone, const QDate &date);
    void populateGroups(vector<CCalendar*> &calendars, bool hideDone, const QDate &date);
    void updateCounters(CTodo *todo, const QDate &date);
    QListWidgetItem* createItem(CTodo *todo, int color, const QDate &date) const;

private slots:
    void onTodoActivated(QListWidgetItem *item = 0);
    void onTodoChanged(QListWidgetItem *item);

    void onContextMenuRequested(const QPoint &pos);
    void newTodoForCurrentGroup();
    void editCurrentTodo();
    void cloneCurrentTodo();
    void deleteCurrentTodo();

    void hideDoneTodos(bool hide);
    void enableGroups(bool enable);
    void cleanTodos();
};

#endif // TODOSPLUG_H
