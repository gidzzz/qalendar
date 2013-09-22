#include "AgendaPlug.h"

#include <vector>
#include <map>

#include <QPushButton>

#include <CMulticalendar.h>
#include "CWrapper.h"
#include "ComponentInstance.h"

#include "NewComponentWidget.h"
#include "DatePickDialog.h"

#include "Roles.h"

AgendaPlug::AgendaPlug(QDate date, QWidget *parent) :
    TemporalPlug(parent),
    ui(new Ui::AgendaPlug)
{
    ui->setupUi(this);

    this->setGlobalDate(date);
    this->date = fromGlobalDate(date);

    // Set up menu actions
    QAction *todayAction = new QAction(tr("Jump to today"), this);
    QAction *jumpAction = new QAction(tr("Jump to"), this);
    QAction *eventAction = new QAction(tr("New event"), this);
    actions.append(todayAction);
    actions.append(jumpAction);
    actions.append(eventAction);
    connect(todayAction, SIGNAL(triggered()), this, SLOT(gotoToday()));
    connect(jumpAction, SIGNAL(triggered()), this, SLOT(selectDay()));
    connect(eventAction, SIGNAL(triggered()), ui->componentList, SLOT(newEvent()));
}

AgendaPlug::~AgendaPlug()
{
    cleanup();
}

void AgendaPlug::onChange()
{
    setDate(currentDateLock ? QDate::currentDate() : this->globalDate());
}

void AgendaPlug::cleanup()
{
    for (int i = 0; i < ui->componentList->count(); i++)
        delete qvariant_cast<ComponentInstance*>(ui->componentList->item(i)->data(ComponentRole));
    ui->componentList->clear();

    for (unsigned int i = 0; i < components.size(); i++)
        delete components[i];
    components.clear();
}

void AgendaPlug::reload()
{
    cleanup();

    ui->componentList->setDate(this->date);

    const int daysBack = 7;
    const int daysForward = 14;
    const int totalDays = daysBack + 1 + daysForward;

    QListWidgetItem *todaysHeading = NULL;

    QDateTime date = QDateTime(this->date).addDays(-daysBack);

    const time_t startStamp = date.toTime_t();
    const time_t   endStamp = date.addDays(totalDays).toTime_t() - 1;
    vector<ComponentInstance*> instances;

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();
    map<int,int> palette;

    // Get components from each calendar
    for (unsigned int i = 0; i < calendars.size(); i++) {
        CCalendar *calendar = calendars[i];
        if (!calendar->IsShown()) continue;

        palette[calendar->getCalendarId()] = calendar->getCalendarColor();

        int offset = 0;
        int error;
        vector<CComponent*> componentsPart = calendar->getAllEventsTodos(startStamp, endStamp, 1024, offset, error);

        for (unsigned int c = 0; c < componentsPart.size(); c++) {
            if (componentsPart[c]->getType() == E_TODO
            &&  static_cast<CTodo*>(componentsPart[c])->getStatus())
            {
                delete componentsPart[c];
            } else {
                components.push_back(componentsPart[c]);
            }
        }
    }

    CWrapper::expand(components, instances, startStamp, endStamp);
    CWrapper::sort(instances);

    // Populate the list day by day
    for (int d = 0; d < totalDays; d++) {
        const bool onToday = date.date() == this->date;
        const time_t nextDayStamp = date.addDays(1).toTime_t();

        if (onToday || !instances.empty() && instances.front()->stamp < nextDayStamp) {
            // Add date heading
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(DateRole, date.date());
            item->setData(HeadingRole, true);
            ui->componentList->addItem(item);

            if (onToday) {
                todaysHeading = item;

                // Add component creation buttons
                NewComponentWidget *ncw = new NewComponentWidget();
                item = new QListWidgetItem();
                item->setData(DateRole, this->date);
                ui->componentList->addItem(item);
                ui->componentList->setItemWidget(item, ncw);

                connect(ncw->eventButton, SIGNAL(clicked()), ui->componentList, SLOT(newEvent()));
                connect(ncw->todoButton,  SIGNAL(clicked()), ui->componentList, SLOT(newTodo()));
            }
        }

        unsigned int i = 0;
        while (i < instances.size() && instances[i]->stamp < nextDayStamp) {
            // Add the intance to the list
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(ComponentRole, QVariant::fromValue(instances[i]));
            item->setData(ColorRole, palette[instances[i]->component->getCalendarId()]);
            item->setData(DateRole, date.date());
            ui->componentList->addItem(item);

            if (instances[i]->end() < nextDayStamp
            ||  instances[i]->end() == nextDayStamp && instances[i]->duration() > 0)
            {
                // If this is the last day of this instance, remove it from the list...
                instances.erase(instances.begin() + i);
            } else {
                // ...otherwise replace it with a copy to use for the next day
                if (nextDayStamp < endStamp)
                    instances[i] = new ComponentInstance(instances[i]->component, instances[i]->stamp);
                i++;
            }
        }

        date = date.addDays(1);
    }

    mc->releaseListCalendars(calendars);

    ui->componentList->scrollToItem(todaysHeading, QAbstractItemView::PositionAtTop);
}

void AgendaPlug::setDate(QDate date)
{
    this->sync();

    this->setGlobalDate(date);
    this->date = fromGlobalDate(date);

    currentDateLock = this->date == fromGlobalDate(QDate::currentDate());;

    reload();
}

void AgendaPlug::selectDay()
{
    DatePickDialog *dpd = new DatePickDialog(DatePickDialog::Day, this->date, this);
    if (dpd->exec() == QDialog::Accepted)
        setDate(dpd->date());
    delete dpd;
}

void AgendaPlug::gotoToday()
{
    setDate(QDate::currentDate());
}
