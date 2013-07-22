#include "MainWindow.h"

#include "Rotator.h"

MainWindow::MainWindow()
{
    this->setAttribute(Qt::WA_Maemo5StackedWindow);

    // Layout
    QWidget *centralWidget = new QWidget();
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setCentralWidget(centralWidget);

    QDate date = QDate::currentDate();

    // Create plugs
    monthPlug = new MonthPlug(date, this);
    weekPlug = new WeekPlug(date, this);
    agendaPlug = new AgendaPlug(date, this);
    todosPlug = new TodosPlug(this);
    journalsPlug = new JournalsPlug(this);

    // Handle week button presses from the month plug
    connect(monthPlug, SIGNAL(weekActivated(QDate)), this, SLOT(showWeek(QDate)));

    // Add plugs
    mainLayout->addWidget(monthPlug);
    mainLayout->addWidget(weekPlug);
    mainLayout->addWidget(agendaPlug);
    mainLayout->addWidget(todosPlug);
    mainLayout->addWidget(journalsPlug);

    // Create the menu
    QMenuBar *menubar = new QMenuBar(this);
    windowMenu = new QMenu(menubar);
    this->setMenuBar(menubar);

    // Create the group for plug actions
    plugGroup = new QActionGroup(this);
    plugGroup->setExclusive(true);

    // Create plug actions in the group
    monthAction = new QAction(tr("Month"), plugGroup);
    weekAction = new QAction(tr("Week"), plugGroup);
    agendaAction = new QAction(tr("Agenda"), plugGroup);
    todosAction = new QAction(tr("Tasks"), plugGroup);
    journalsAction = new QAction(tr("Notes"), plugGroup);
    settingsAction = new QAction(tr("Settings"), this);

    // Make plug actions checkable
    monthAction->setCheckable(true);
    weekAction->setCheckable(true);
    agendaAction->setCheckable(true);
    todosAction->setCheckable(true);
    journalsAction->setCheckable(true);

    // Connect the actions to their slots
    connect(monthAction, SIGNAL(triggered()), this, SLOT(showMonth()));
    connect(weekAction, SIGNAL(triggered()), this, SLOT(showWeek()));
    connect(agendaAction, SIGNAL(triggered()), this, SLOT(showAgenda()));
    connect(todosAction, SIGNAL(triggered()), this, SLOT(showTodos()));
    connect(journalsAction, SIGNAL(triggered()), this, SLOT(showJournals()));

    connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettings()));

    menubar->addAction(windowMenu->menuAction());

    deleteOldComponents();

    Rotator::acquire()->setSlave(this);

    // Select the last used plug
    QString view = QSettings().value("View", "month").toString();
    if (view == "month")
        monthAction->trigger();
    else if (view == "week")
        weekAction->trigger();
    else if (view == "agenda")
        agendaAction->trigger();
    else if (view == "todos")
        todosAction->trigger();
    else if (view == "journals")
        journalsAction->trigger();
}

// Forward activation events to the current plug
void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::ActivationChange && this->isActiveWindow())
        currentPlug->onActivated();

    QMainWindow::changeEvent(e);
}

// Change current plug to the provided one
void MainWindow::setPlug(Plug *plug)
{
    // Disconnect all plugs
    for (int i = 0; i < mainLayout->count(); i++) {
        mainLayout->itemAt(i)->widget()->hide();
        disconnect(mainLayout->itemAt(i)->widget(), SIGNAL(titleChanged(QString)), this, SLOT(setWindowTitle(QString)));
    }

    // Connect the chosen plug
    Rotator::acquire()->setPolicy(plug->isRotatable() ? Rotator::Automatic : Rotator::Landscape);
    connect(plug, SIGNAL(titleChanged(QString)), this, SLOT(setWindowTitle(QString)));
    this->setWindowTitle(plug->title());
    plug->show();

    // Repopulate the menu
    windowMenu->clear();
    windowMenu->addActions(plugGroup->actions());
    for (int i = 0; i < plug->actions.size(); i++)
        windowMenu->addAction(plug->actions.at(i));
    windowMenu->addAction(settingsAction);

    currentPlug = plug;
    currentPlug->onActivated();
}

// Delete events and todos older than specified in the settings
void MainWindow::deleteOldComponents()
{
    int eventsSecondsBack = QSettings().value("DeleteEventsAfter", 0).toInt();
    int  todosSecondsBack = QSettings().value("DeleteTodosAfter",  0).toInt();

    if (eventsSecondsBack == 0 && todosSecondsBack == 0) return;

    const time_t eventsCutOffStamp = QDateTime::currentDateTime().toTime_t() - eventsSecondsBack;
    const time_t  todosCutOffStamp = QDateTime::currentDateTime().toTime_t() - todosSecondsBack;

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();

    for (unsigned int i = 0; i < calendars.size(); i++) {
        if (calendars[i]->getCalendarType() == BIRTHDAY_CALENDAR) continue;

        int error;

        if (eventsSecondsBack > 0)
            mc->deleteEventBeyond(calendars[i]->getCalendarId(), eventsCutOffStamp, error);
        if (todosSecondsBack > 0)
            mc->deleteTodosBeyond(calendars[i]->getCalendarId(), todosCutOffStamp, error);
    }

    mc->releaseListCalendars(calendars);
}

// Switch to month view
void MainWindow::showMonth()
{
    setPlug(monthPlug);

    QSettings().setValue("View", "month");
}

// Switch to week view
void MainWindow::showWeek(QDate date)
{
    setPlug(weekPlug);

    if (date.isValid())
        weekPlug->setDate(date);

    QSettings().setValue("View", "week");
}

// Switch to agenda view
void MainWindow::showAgenda()
{
    setPlug(agendaPlug);

    QSettings().setValue("View", "agenda");
}

// Switch to tasks view
void MainWindow::showTodos()
{
    setPlug(todosPlug);

    QSettings().setValue("View", "todos");
}

// Switch to notes view
void MainWindow::showJournals()
{
    setPlug(journalsPlug);

    QSettings().setValue("View", "journals");
}

// Open the settings dialog
void MainWindow::openSettings()
{
    (new SettingsDialog(this))->exec();
}
