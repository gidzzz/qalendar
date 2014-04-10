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
    QAction *cleanAction = new QAction(tr("Delete completed"), this);
    hideAction->setCheckable(true);
    hideAction->setChecked(settings.value("HideDone", false).toBool());
    actions.append(hideAction);
    actions.append(cleanAction);
    connect(hideAction, SIGNAL(toggled(bool)), this, SLOT(hideDoneTodos(bool)));
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

    return tr("Tasks") + status;
}

void TodosPlug::onChange()
{
    reload();
}

void TodosPlug::reload()
{
    this->sync();

    const int scrollPosition = ui->todoList->verticalScrollBar()->value();

    while (ui->todoList->count() > 1) {
        QListWidgetItem *item = ui->todoList->item(1);
        delete qvariant_cast<CTodo*>(item->data(TodoRole));
        delete item;
    }

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();
    vector<CTodo*> todos;
    map<int,int> palette;

    // Iterate over a list of calendars to get all todo items
    for (unsigned int i = 0; i < calendars.size(); i++) {
        if (!calendars[i]->IsShown()) continue;
        int error;
        vector<CTodo*> todosPart = calendars[i]->getTodos(error);
        for (unsigned int t = 0; t < todosPart.size(); t++)
            todosPart[t]->setCalendarId(calendars[i]->getCalendarId()); // This is not done automatically

        todos.insert(todos.end(), todosPart.begin(), todosPart.end());

        palette[calendars[i]->getCalendarId()] = calendars[i]->getCalendarColor();
    }

    mc->releaseListCalendars(calendars);

    CWrapper::sort(todos);

    // The date used for marking overdue tasks
    QDate date = QDate::currentDate();

    // Load display settings
    QSettings settings;
    settings.beginGroup("TodosPlug");
    const bool hideDone = settings.value("HideDone", false).toBool();

    numOverdue = 0;
    numUndone = 0;
    numTotal = todos.size();

    // Iterate over a list of todos and add them to the list widget
    for (unsigned int i = 0; i < todos.size(); i++) {
        if (hideDone && todos[i]->getStatus()) {
            delete todos[i];
        } else {
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(TodoRole, QVariant::fromValue(todos[i]));
            item->setData(ColorRole, palette[todos[i]->getCalendarId()]);
            item->setData(DateRole, date);
            item->setCheckState(todos[i]->getStatus() ? Qt::Checked : Qt::Unchecked);
            ui->todoList->addItem(item);

            if (!todos[i]->getStatus()) {
                numUndone++;
                if (QDateTime::fromTime_t(todos[i]->getDue()).date() < date) {
                    numOverdue++;
                }
            }
        }
    }

    // Restore the original view
    ui->todoList->verticalScrollBar()->setValue(scrollPosition);

    emit titleChanged(title());
}

void TodosPlug::onTodoActivated(QListWidgetItem *item)
{
    CTodo *todo = NULL;

    if (item)
        todo = qvariant_cast<CTodo*>(item->data(TodoRole));

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

    QMenu *contextMenu = new QMenu(this);
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    contextMenu->addAction(tr("Edit"), this, SLOT(editCurrentTodo()));
    contextMenu->addAction(tr("Delete"), this, SLOT(deleteCurrentTodo()));
    contextMenu->exec(this->mapToGlobal(pos));
}

void TodosPlug::editCurrentTodo()
{
    ChangeManager::edit(this, qvariant_cast<CTodo*>(ui->todoList->currentItem()->data(TodoRole)));
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
