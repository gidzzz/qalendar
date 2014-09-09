#include "TodosPlug.h"

#include <limits>

#include <QMenu>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollBar>

#include <QDateTime>
#include <QSettings>

#include <CMulticalendar.h>
#include <CTodo.h>

#include "TodoWindow.h"
#include "TodoDelegate.h"

#include "CWrapper.h"

#include "ChangeManager.h"

TodosPlug::TodosPlug(QWidget *parent) :
    Plug(parent),
    ui(new Ui::TodosPlug)
{
    ui->setupUi(this);

    ui->todoList->setItemDelegate(new TodoDelegate(ui->todoList));

    this->setAttribute(Qt::WA_Maemo5StackedWindow);

    // Create a button to add new tasks
    QPushButton *newTodoButton = new QPushButton(QIcon::fromTheme("general_add"), tr("New task"));
    QListWidgetItem *item = new QListWidgetItem(ui->todoList);
    ui->todoList->setItemWidget(item, newTodoButton);

    QSettings settings;
    settings.beginGroup("TodosPlug");

    QAction *hideAction = new QAction(tr("Hide completed"), this);
    QAction *groupAction = new QAction(tr("Group by calendar"), this);
    QAction *cleanAction = new QAction(tr("Delete completed"), this);
    hideAction->setCheckable(true);
    groupAction->setCheckable(true);
    hideAction->setChecked(settings.value("HideDone", false).toBool());
    groupAction->setChecked(settings.value("GroupByCalendar", false).toBool());
    actions.append(hideAction);
    actions.append(groupAction);
    actions.append(cleanAction);
    connect(hideAction, SIGNAL(toggled(bool)), this, SLOT(hideDoneTodos(bool)));
    connect(groupAction, SIGNAL(toggled(bool)), this, SLOT(enableGroups(bool)));
    connect(cleanAction, SIGNAL(triggered()), this, SLOT(cleanTodos()));

    connect(newTodoButton, SIGNAL(clicked()), this, SLOT(onTodoActivated()));
    connect(ui->todoList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onTodoActivated(QListWidgetItem*)));
    connect(ui->todoList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onTodoChanged(QListWidgetItem*)));
    connect(ui->todoList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)));
}

TodosPlug::~TodosPlug()
{
    for (int i = 1; i < ui->todoList->count(); i++)
        delete qvariant_cast<CTodo*>(ui->todoList->item(i)->data(TodoRole));

    delete ui;
}

QString TodosPlug::title() const
{
    return tr("Tasks") + statusString(numOverdue, numUndone, numTotal);
}

void TodosPlug::onChange()
{
    reload();
}

QString TodosPlug::statusString(int numOverdue, int numUndone, int numTotal) const
{
    QString status;

    if (numTotal) {
        status = QString::number(numTotal) + ")";
        if (numUndone) {
            status = QString::number(numUndone) + "/" + status;
            if (numOverdue) {
                status = QString::number(numOverdue) + "! " + status;
            }
        }
        status = " (" + status;
    }

    return status;
}

void TodosPlug::reload()
{
    this->sync();

    // Save current view
    const int scrollPosition = ui->todoList->verticalScrollBar()->value();

    // Clear the existing items
    for (int i = ui->todoList->count()-1; i > 0; i--) {
        QListWidgetItem *item = ui->todoList->item(i);
        delete qvariant_cast<CTodo*>(item->data(TodoRole));
        delete item;
    }
    numOverdue = 0;
    numUndone = 0;
    numTotal = 0;

    // Load display settings
    QSettings settings;
    settings.beginGroup("TodosPlug");
    const bool hideDone = settings.value("HideDone", false).toBool();
    const bool enableGroups = settings.value("GroupByCalendar", false).toBool();

    // The date used for marking overdue tasks
    const QDate date = QDate::currentDate();

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();

    enableGroups ? populateGroups(calendars, hideDone, date)
                 : populateSingle(calendars, hideDone, date);

    mc->releaseListCalendars(calendars);

    // Restore the original view
    ui->todoList->verticalScrollBar()->setValue(scrollPosition);

    // The title could have changed
    emit titleChanged(title());
}

void TodosPlug::populateSingle(vector<CCalendar*> &calendars, bool hideDone, const QDate &date)
{
    vector<CTodo*> todos;
    map<int,int> palette;

    // Collect todos from all calendars
    for (unsigned int c = 0; c < calendars.size(); c++) {
        // Skip hidden calendars
        if (!calendars[c]->IsShown()) continue;

        // Get all todos from the calendar
        int error;
        vector<CTodo*> todosPart = calendars[c]->getTodos(error);
        for (unsigned int t = 0; t < todosPart.size(); t++)
            todosPart[t]->setCalendarId(calendars[c]->getCalendarId()); // This is not done automatically

        // Add the todos to the combined list
        todos.insert(todos.end(), todosPart.begin(), todosPart.end());

        // Remember calendar color
        palette[calendars[c]->getCalendarId()] = calendars[c]->getCalendarColor();
    }

    CWrapper::sort(todos);

    // Display the collected todos
    for (unsigned int t = 0; t < todos.size(); t++) {
        if (hideDone && todos[t]->getStatus()) {
            // Reject and delete
            delete todos[t];
        } else {
            // Accept for display
            ui->todoList->addItem(createItem(todos[t], palette[todos[t]->getCalendarId()], date));
        }
        updateCounters(todos[t], date);
    }
}

void TodosPlug::populateGroups(vector<CCalendar*> &calendars, bool hideDone, const QDate &date)
{
    QSet<int> groups;
    int lastOverdue = 0;
    int lastUndone = 0;
    int lastTotal = 0;

    CWrapper::sort(calendars, false);

    // Create a group from each calendar
    for (unsigned int c = 0; c < calendars.size(); c++) {
        // Skip hidden calendars
        if (!calendars[c]->IsShown()) continue;

        // Get all todos from the calendar
        int error;
        vector<CTodo*> todosPart = calendars[c]->getTodos(error);
        CWrapper::sort(todosPart);

        QListWidgetItem *heading = NULL;
        int groupSize = 0;

        // Display collected todos
        for (unsigned int t = 0; t < todosPart.size(); t++) {
            if (hideDone && todosPart[t]->getStatus()) {
                // Reject and delete
                delete todosPart[t];
            } else {
                // Accept for display
                todosPart[t]->setCalendarId(calendars[c]->getCalendarId()); // This is not done automatically

                // Add a heading if it does not exist yet
                if (!heading) {
                    heading = new QListWidgetItem();
                    heading->setFlags(heading->flags() & ~Qt::ItemIsUserCheckable);
                    heading->setData(HeadingRole, true);
                    heading->setData(IdRole, calendars[c]->getCalendarId());
                    heading->setData(NameRole, QString::fromUtf8(calendars[c]->getCalendarName().c_str()));
                    heading->setData(ColorRole, calendars[c]->getCalendarColor());
                    ui->todoList->addItem(heading);

                    groups.insert(calendars[c]->getCalendarId());
                }

                // Add todo item
                QListWidgetItem *item = createItem(todosPart[t], calendars[c]->getCalendarColor(), date);
                ui->todoList->addItem(item);
                if (!visibleGroups.contains(calendars[c]->getCalendarId()))
                    item->setHidden(true);
                groupSize++;
            }
            updateCounters(todosPart[t], date);
        }

        // Update the heading and counters
        if (heading) {
            heading->setData(TodoCountRole, groupSize);
            heading->setData(DetailsRole, statusString(numOverdue - lastOverdue,
                                          numUndone - lastUndone,
                                          numTotal - lastTotal));

            lastOverdue = numOverdue;
            lastUndone = numUndone;
            lastTotal = numTotal;
        }
    }

    // Remove old, nonexistent groups
    visibleGroups.intersect(groups);
}

void TodosPlug::updateCounters(CTodo *todo, const QDate &date)
{
    numTotal++;

    if (!todo->getStatus()) {
        numUndone++;
        if (QDateTime::fromTime_t(todo->getDue()).date() < date)
            numOverdue++;
    }
}

QListWidgetItem* TodosPlug::createItem(CTodo *todo, int color, const QDate &date) const
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setData(TodoRole, QVariant::fromValue(todo));
    item->setData(ColorRole, color);
    item->setData(DateRole, date);
    item->setCheckState(todo->getStatus() ? Qt::Checked : Qt::Unchecked);
    return item;
}

void TodosPlug::onTodoActivated(QListWidgetItem *item)
{
    CTodo *todo = NULL;

    if (item) {
        if (item->data(HeadingRole).toBool()) {
            // Group heading -- toggle visibility
            const int id = item->data(IdRole).toInt();
            const int size = item->data(TodoCountRole).toInt();
            const bool hide = visibleGroups.contains(id);

            for (int i = 1; i <= size; i++)
                ui->todoList->item(ui->todoList->row(item)+i)->setHidden(hide);

            if (hide) {
                visibleGroups.remove(id);
            } else {
                visibleGroups.insert(id);
            }

            return;
        } else {
            // Todo item -- get the underlying object
            todo = qvariant_cast<CTodo*>(item->data(TodoRole));
        }
    }

    if (todo) {
        (new TodoWindow(todo, this))->show();
    } else {
        ChangeManager::newTodo(this);
    }
}

void TodosPlug::onTodoChanged(QListWidgetItem *item)
{
    // Get the todo representation
    CTodo *todo = qvariant_cast<CTodo*>(item->data(TodoRole));

    // Abort on null item
    if (!todo) return;

    // Modify the status
    todo->setStatus(item->checkState() == Qt::Checked);

    // Remove the alarm
    todo->removeAlarm();

    // Make changes persistent
    ChangeManager::save(todo);

    // NOTE: Reloading the whole list might be bad for performance reasons,
    // but it should at least be sorted. Reloading the whole list also has
    // an unpleasant side effect of resetting scrollbar position.
    reload();
}

void TodosPlug::onContextMenuRequested(const QPoint &pos)
{
    if (ui->todoList->currentItem()->data(TodoRole).isNull()) return;

    QMenu contextMenu(this);
    contextMenu.addAction(tr("Edit"), this, SLOT(editCurrentTodo()));
    contextMenu.addAction(tr("Clone"), this, SLOT(cloneCurrentTodo()));
    contextMenu.addAction(tr("Delete"), this, SLOT(deleteCurrentTodo()));
    contextMenu.exec(this->mapToGlobal(pos));
}

void TodosPlug::editCurrentTodo()
{
    ChangeManager::edit(this, qvariant_cast<CTodo*>(ui->todoList->currentItem()->data(TodoRole)));
}

void TodosPlug::cloneCurrentTodo()
{
    ChangeManager::clone(this, qvariant_cast<CTodo*>(ui->todoList->currentItem()->data(TodoRole)));
}

void TodosPlug::deleteCurrentTodo()
{
    ChangeManager::drop(this, qvariant_cast<CTodo*>(ui->todoList->currentItem()->data(TodoRole)));
}

void TodosPlug::hideDoneTodos(bool hide)
{
    QSettings settings;
    settings.beginGroup("TodosPlug");
    settings.setValue("HideDone", hide);

    reload();
}

void TodosPlug::enableGroups(bool enable)
{
    QSettings settings;
    settings.beginGroup("TodosPlug");
    settings.setValue("GroupByCalendar", enable);

    reload();
}

void TodosPlug::cleanTodos()
{
    if (QMessageBox::warning(this, " ", tr("Delete completed tasks from all calendars?"),
                             QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel) == QMessageBox::Yes)
    {
        // NOTE: It might be convenient to have an option to ignore invisible
        // calendars.

        CMulticalendar *mc = CMulticalendar::MCInstance();
        vector<CCalendar*> calendars = mc->getListCalFromMc();

        for (unsigned int i = 0; i < calendars.size(); i++) {
            if (calendars[i]->getCalendarType() == BIRTHDAY_CALENDAR) continue;

            int error;

            vector<CTodo*> todos = calendars[i]->getTodos(error);
            for (unsigned int t = 0; t < todos.size(); t++) {
                if (todos[t]->getStatus())
                    mc->deleteTodo(calendars[i]->getCalendarId(), todos[t]->getId(), error);
                delete todos[t];
            }
        }

        mc->releaseListCalendars(calendars);

        ChangeManager::bump();
    }
}
