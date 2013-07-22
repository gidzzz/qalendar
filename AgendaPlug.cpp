#include "AgendaPlug.h"

#include <vector>
#include <map>

#include <QPushButton>

#include <CMulticalendar.h>
#include "CWrapper.h"
#include "ComponentInstance.h"

#include "Roles.h"

#include "DatePickDialog.h"

AgendaPlug::AgendaPlug(QDate date, QWidget *parent) :
    TemporalPlug(parent),
    ui(new Ui::AgendaPlug),
    date(date)
{
    ui->setupUi(this);

    this->setGlobalDate(date);

    // Set up menu actions
    QAction *todayAction = new QAction(tr("Jump to today"), this);
    QAction *jumpAction = new QAction(tr("Jump to"), this);
    QAction *eventAction = new QAction(tr("New event"), this);
    actions.append(todayAction);
    actions.append(jumpAction);
    actions.append(eventAction);
    connect(todayAction, SIGNAL(triggered()), this, SLOT(gotoToday()));
    connect(jumpAction, SIGNAL(triggered()), this, SLOT(selectDay()));
    connect(eventAction, SIGNAL(triggered()), ui->componentList, SLOT(onComponentActivated()));
}

AgendaPlug::~AgendaPlug()
{
    cleanup();
}

void AgendaPlug::onActivated()
{
    if (this->isOutdated()
    ||  this->globalDate() != fromGlobalDate(date))
    {
        setDate(this->globalDate()); // Reload
    }
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

    const int daysBack = 7;
    const int daysForward = 14;
    const int totalDays = daysBack + 1 + daysForward;

    QListWidgetItem *todaysHeading = NULL;

    QDateTime date = QDateTime(this->date).addDays(-daysBack);

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();
    map<int,int> palette;

    // Get events for each day
    for (int d = 0; d < totalDays; d++) {
        const time_t dayStartStamp = date.toTime_t();
        const time_t   dayEndStamp = date.addDays(1).toTime_t() - 1;
        vector<ComponentInstance*> instances;

        // Get events from each calendar
        for (unsigned int i = 0; i < calendars.size(); i++) {
            CCalendar *calendar = calendars[i];
            if (!calendar->IsShown()) continue;

            palette[calendar->getCalendarId()] = calendar->getCalendarColor();

            int offset = 0;
            int error;
            vector<CComponent*> componentsPart = calendar->getAllEventsTodos(dayStartStamp, dayEndStamp, 1024, offset, error);

            for (unsigned int c = 0; c < componentsPart.size(); c++) {
                if (componentsPart[c]->getType() == E_TODO
                && (QDateTime::fromTime_t(componentsPart[c]->getDateStart()).date() != date.date() ||
                    static_cast<CTodo*>(componentsPart[c])->getStatus()))
                {
                    delete componentsPart[c];
                    componentsPart[c] = NULL;
                } else {
                    components.push_back(componentsPart[c]);
                }
            }

            CWrapper::expand(componentsPart, instances, dayStartStamp, dayEndStamp);
        }

        CWrapper::sort(instances);

        const bool onToday = date.date() == this->date;

        // Add day heading
        if (onToday || !instances.empty()) {
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(DateRole, date.date());
            item->setData(HeadingRole, true);
            ui->componentList->addItem(item);

            if (onToday) {
                todaysHeading = item;
                // Create and add 'new event' button to the list
                QPushButton *newEventButton = new QPushButton(ui->componentList);
                newEventButton->setText("New event");
                newEventButton->setIcon(QIcon::fromTheme("general_add"));
                item = new QListWidgetItem();
                item->setData(DateRole, this->date);
                ui->componentList->addItem(item);
                ui->componentList->setItemWidget(item, newEventButton);
                connect(newEventButton, SIGNAL(clicked()), ui->componentList, SLOT(onComponentActivated()));
            }
        }

        // Add the components for the day
        for (unsigned int i = 0; i < instances.size(); i++) {
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(ComponentRole, QVariant::fromValue(instances[i]));
            item->setData(ColorRole, palette[instances[i]->component->getCalendarId()]);
            item->setData(DateRole, date.date());
            ui->componentList->addItem(item);
        }

        // Go to the next day
        date = date.addDays(1);
    }

    mc->releaseListCalendars(calendars);

    ui->componentList->scrollToItem(todaysHeading, QAbstractItemView::PositionAtTop);
}

void AgendaPlug::setDate(QDate date)
{
    this->sync();
    this->setGlobalDate(date);

    this->date = date;
    ui->componentList->setDate(date);
    reload();
}

void AgendaPlug::selectDay()
{
    DatePickDialog *dpd = new DatePickDialog(DatePickDialog::Day, date, this);
    if (dpd->exec() == QDialog::Accepted)
        setDate(dpd->date());
    delete dpd;
}

void AgendaPlug::gotoToday()
{
    setDate(QDate::currentDate());
}
