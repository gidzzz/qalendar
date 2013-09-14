#include "DayWindow.h"

#include <iostream>

#include <map>
#include <vector>

#include <QPushButton>
#include <QShortcut>

#include "DatePickDialog.h"

#include <CMulticalendar.h>
#include <CTodo.h>
#include "CWrapper.h"

#include "ComponentListWidget.h"

#include "Roles.h"
#include "Date.h"

DayWindow::DayWindow(QDate date, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DayWindow)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAttribute(Qt::WA_Maemo5StackedWindow);

    this->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
    this->setAttribute(Qt::WA_Maemo5LandscapeOrientation, false);
    this->setAttribute(Qt::WA_Maemo5PortraitOrientation, false);

    // Create a button to add new tasks
    QPushButton *newEventButton = new QPushButton(QIcon::fromTheme("general_add"), tr("New event"));
    QListWidgetItem *item = new QListWidgetItem(ui->componentList);
    ui->componentList->setItemWidget(item, newEventButton);

    connect(newEventButton, SIGNAL(clicked()), ui->componentList, SLOT(onComponentActivated()));

    connect(ui->prevAction, SIGNAL(triggered()), this, SLOT(gotoPrevDay()));
    connect(ui->nextAction, SIGNAL(triggered()), this, SLOT(gotoNextDay()));
    connect(ui->todayAction, SIGNAL(triggered()), this, SLOT(gotoToday()));
    connect(ui->jumpAction, SIGNAL(triggered()), this, SLOT(selectDay()));

    connect(new QShortcut(QKeySequence(Qt::Key_Left), this), SIGNAL(activated()), this, SLOT(gotoPrevDay()));
    connect(new QShortcut(QKeySequence(Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(gotoNextDay()));

    connect(new QShortcut(QKeySequence(Qt::Key_Backspace), this), SIGNAL(activated()), this, SLOT(close()));

    setDate(date);
}

DayWindow::~DayWindow()
{
    cleanup();
}

void DayWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::ActivationChange) {
        if (this->isActiveWindow()) {
            if (this->isOutdated())
                onChange();

            this->activate();
        } else {
            this->deactivate();
        }
    }

    QMainWindow::changeEvent(e);
}

void DayWindow::onChange()
{
    reload();
}

void DayWindow::cleanup()
{
    // Clear the list widget, but leave the button in place
    while (ui->componentList->count() > 1) {
        QListWidgetItem *item = ui->componentList->item(1);
        delete qvariant_cast<ComponentInstance*>(item->data(ComponentRole));
        delete item;
    }

    for (unsigned int i = 0; i < components.size(); i++)
        delete components[i];
    components.clear();
}

void DayWindow::reload()
{
    this->sync();

    cleanup();

    // Update window title
    this->setWindowTitle(Date::toString(date, Date::Full));

    const time_t startStamp = QDateTime(date).toTime_t();
    const time_t   endStamp = QDateTime(date.addDays(1)).toTime_t() - 1;

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();
    map<int, int> palette;

    // Get events from each calendar
    for (unsigned int i = 0; i < calendars.size(); i++) {
        CCalendar *calendar = calendars[i];

        if (!calendar->IsShown()) continue;

        palette[calendar->getCalendarId()] = calendar->getCalendarColor();

        int offset = 0;
        int error;
        vector<CComponent*> componentsPart = calendar->getAllEventsTodos(startStamp, endStamp, 1024, offset, error);

        for (unsigned int c = 0; c < componentsPart.size(); c++) {
            if (componentsPart[c]->getType() == E_TODO
            && (QDateTime::fromTime_t(componentsPart[c]->getDateStart()).date() != date ||
                static_cast<CTodo*>(componentsPart[c])->getStatus()))
            {
                delete componentsPart[c];
            } else {
                components.push_back(componentsPart[c]);
            }
        }
    }

    mc->releaseListCalendars(calendars);

    vector<ComponentInstance*> instances;
    CWrapper::expand(components, instances, startStamp, endStamp);
    CWrapper::sort(instances);

    for (unsigned int i = 0; i < instances.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(ComponentRole, QVariant::fromValue(instances[i]));
        item->setData(ColorRole, palette[instances[i]->component->getCalendarId()]);
        item->setData(DateRole, date);
        ui->componentList->addItem(item);
    }
}

void DayWindow::setDate(QDate date)
{
    this->date = date;
    ui->componentList->setDate(date);
    reload();
}

void DayWindow::selectDay()
{
    DatePickDialog *dpd = new DatePickDialog(DatePickDialog::Day, date, this);
    if (dpd->exec() == QDialog::Accepted)
        setDate(dpd->date());
    delete dpd;
}

void DayWindow::gotoPrevDay()
{
    setDate(date.addDays(-1));
}

void DayWindow::gotoNextDay()
{
    setDate(date.addDays(1));
}

void DayWindow::gotoToday()
{
    setDate(QDate::currentDate());
}
