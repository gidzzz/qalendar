#include "ChangeManager.h"

#include <QMessageBox>
#include <QMaemo5InformationBox>

#include <QDBusConnection>

#include <CalendarErrors.h>

#include "EventEditDialog.h"
#include "TodoEditDialog.h"
#include "JournalEditDialog.h"
#include "BirthdayEditDialog.h"

#include "ChangeClient.h"

#include "MainWindow.h"
#include "CWrapper.h"

Version ChangeManager::m_version = 1;
QDate ChangeManager::m_date = QDate::currentDate();

bool ChangeManager::birthdayCalendarEnabled = false;
BirthdayCalendar* ChangeManager::birthdayCalendar = NULL;

ChangeClient* ChangeManager::activeClient = NULL;
QTimer* ChangeManager::dateCheckTimer;

ChangeManager* ChangeManager::instance = new ChangeManager();

ChangeManager::ChangeManager()
{
    dateCheckTimer = new QTimer(this);
    dateCheckTimer->setInterval(10000);
    connect(dateCheckTimer, SIGNAL(timeout()), this, SLOT(checkDate()));

    QDBusConnection::sessionBus().connect("", "", DBUS_INTERFACE, "dbChange",
                                          this, SLOT(onDbChange(QString,QString)));
}

// Enable management of the birthday calendar
void ChangeManager::enableBirthdayCalendar()
{
    birthdayCalendarEnabled = true;

    onDbChange("CALENDAR", CMulticalendar::MCInstance()->getApplicationName().c_str());
}

// Start delivering change notifications to the specified client
void ChangeManager::activateClient(ChangeClient *client)
{
    activeClient = client;

    // Check the date, which might result in version bumping
    checkDate();
    dateCheckTimer->start();

    // If the date check did not result in a bump and the associated onChange(),
    // the client still might have old data, in which case onChange() still
    // needs to be called.
    if (client->isOutdated())
        client->onChange();
}

// Stop change notification delivery
void ChangeManager::deactivateClient(ChangeClient *client)
{
    if (activeClient == client) {
        activeClient = NULL;
        dateCheckTimer->stop();
    }
}

// Return the version of tracked database state
Version ChangeManager::version()
{
    return m_version;
}

// Check if an update should occur due to passing time
void ChangeManager::checkDate()
{
    const QDate currentDate = QDate::currentDate();

    if (m_date != currentDate) {
        m_date = currentDate;
        bump();
    }
}

// Handle a database change notification from D-Bus
void ChangeManager::onDbChange(QString details, QString appName)
{
    // Check if everything is alright with the birthday calendar after each
    // calendar change.
    if (birthdayCalendarEnabled && details.contains("CALENDAR")) {
        CCalendar *calendar = CMulticalendar::MCInstance()->getBirthdayCalendar();

        if (birthdayCalendar) {
            if (calendar) {
                if (calendar->getCalendarId() == birthdayCalendar->id()) {
                    // No change
                    delete calendar;
                } else {
                    // Overtaken by another calendar
                    delete birthdayCalendar;
                    birthdayCalendar = new BirthdayCalendar(calendar);
                }
            } else {
                // Removed
                delete birthdayCalendar;
                birthdayCalendar = NULL;
            }
        } else {
            if (calendar) {
                // Created
                birthdayCalendar = new BirthdayCalendar(calendar);
            }
        }
    }

    // Do not react to other updates originating from this application, as they
    // are handled internally.
    // NOTE: This check will not recognize updates coming from a different
    // instance of this application.
    if (appName != CMulticalendar::MCInstance()->getApplicationName().c_str())
        bump();
}

// Bump the database state version
void ChangeManager::bump()
{
    m_version++;

    if (activeClient)
        activeClient->onChange();
}

// Open the component in the appropriate editor
bool ChangeManager::edit(QWidget *parent, CComponent *component)
{
    /*if (!component) return false;*/

    QDialog *ed = NULL;

    switch (component->getType()) {
        case E_EVENT:
            ed = new EventEditDialog(parent, static_cast<CEvent*>(component));
            break;
        case E_TODO:
            ed = new TodoEditDialog(parent, static_cast<CTodo*>(component));
            break;
        case E_JOURNAL:
            ed = new JournalEditDialog(parent, static_cast<CJournal*>(component));
            break;
        case E_BDAY:
            ed = new BirthdayEditDialog(parent, static_cast<CBdayEvent*>(component));
            break;
    }

    return ed && ed->exec() == QDialog::Accepted;
}

// Open a copy of the component in the appropriate editor
bool ChangeManager::clone(QWidget *parent, CComponent *component)
{
    CComponent *clone = NULL;

    switch (component->getType()) {
        case E_EVENT:
            clone = CWrapper::details(static_cast<CEvent*>(component));
            break;
        case E_TODO:
            clone = CWrapper::details(static_cast<CTodo*>(component));
            break;
        case E_JOURNAL:
            clone = CWrapper::details(static_cast<CJournal*>(component));
            break;
        case E_BDAY:
            QMaemo5InformationBox::information(parent, QObject::tr("Unable to edit birthdays"));
            break;
    }

    if (clone) {
        clone->setId(string());
        return edit(parent, clone); // The edit dialog will take the ownership of this clone
    } else {
        return false;
    }
}

// Delete the component
bool ChangeManager::drop(QWidget *parent, CComponent *component)
{
    if (QMessageBox::warning(parent, " ", QObject::tr("Delete component?"),
                             QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel) != QMessageBox::Yes)
    {
        return false;
    }

    int error;

    deleteComponent(component, component->getCalendarId(), error);

    bump();

    return true;
}

// Show editor with a new event
bool ChangeManager::newEvent(QWidget *parent)
{
    EventEditDialog *ed = new EventEditDialog(parent);

    return ed->exec() == QDialog::Accepted;
}

// Show editor with a new event, configure dates
bool ChangeManager::newEvent(QWidget *parent, QDateTime from)
{
    EventEditDialog *ed = new EventEditDialog(parent);

    ed->setFromTo(from, from);

    return ed->exec() == QDialog::Accepted;
}

// Show editor with a new event, configure dates and alldayness
bool ChangeManager::newEvent(QWidget *parent, QDateTime from, QDateTime to, bool allDay)
{
    EventEditDialog *ed = new EventEditDialog(parent);

    ed->setFromTo(from, to);
    ed->setAllDay(allDay);

    return ed->exec() == QDialog::Accepted;
}

// Show editor with a new task, configure deadline
bool ChangeManager::newTodo(QWidget *parent, QDate due)
{
    TodoEditDialog *ed = new TodoEditDialog(parent);

    ed->setDue(due);

    return ed->exec() == QDialog::Accepted;
}

// Show editor with a new task
bool ChangeManager::newTodo(QWidget *parent)
{
    TodoEditDialog *ed = new TodoEditDialog(parent);

    return ed->exec() == QDialog::Accepted;
}

// Show editor with a new note
bool ChangeManager::newJournal(QWidget *parent)
{
    JournalEditDialog *ed = new JournalEditDialog(parent);

    return ed->exec() == QDialog::Accepted;
}

// Low-level saving of a component
bool ChangeManager::save(CComponent *component, int calendarId)
{
    int error;

    if (calendarId == -1)
        calendarId = component->getCalendarId();

    // Check if the component already exists in the database
    if (!component->getId().empty()) {
        if (calendarId == component->getCalendarId()) {
            // Simply save chages
            modifyComponent(component, component->getCalendarId(), error);
        } else {
            // Move the component between calendars, start by removing it from the old one
            deleteComponent(component, component->getCalendarId(), error);
        }
    }

    // Add the component to a calendar if it is a fresh one or has to be moved
    if (component->getId().empty())
        addComponent(component, calendarId, error);

    bump();

    // TODO: Error handling, both here and in edit dialogs
    return true;
}

// Add the component to the calendar (low-level helper)
void ChangeManager::addComponent(CComponent *component, int calendarId, int &error)
{
    component->setCalendarId(calendarId);

    CMulticalendar *mc = CMulticalendar::MCInstance();

    switch (component->getType()) {
        case E_EVENT:
            mc->addEvent(static_cast<CEvent*>(component), calendarId, error);
            break;
        case E_TODO:
            mc->addTodo(static_cast<CTodo*>(component), calendarId, error);
            break;
        case E_JOURNAL:
            mc->addJournal(static_cast<CJournal*>(component), calendarId, error);
            break;
        // TODO: Set error if the type cannot be handled?
    }
}

// Modify the component belonging to the calendar (low-level helper)
void ChangeManager::modifyComponent(CComponent *component, int calendarId, int &error)
{
    CMulticalendar *mc = CMulticalendar::MCInstance();

    switch (component->getType()) {
        case E_EVENT:
            mc->modifyEvent(static_cast<CEvent*>(component), calendarId, error);
            break;
        case E_TODO:
            mc->modifyTodo(static_cast<CTodo*>(component), calendarId, error);
            break;
        case E_JOURNAL:
            mc->modifyJournal(static_cast<CJournal*>(component), calendarId, error);
            break;
        case E_BDAY:
            mc->modifyBirthDay(static_cast<CBdayEvent*>(component), error);
            break;
    }
}

// Delete the component from the calendar (low-level helper)
void ChangeManager::deleteComponent(CComponent *component, int calendarId, int &error)
{
    CMulticalendar *mc = CMulticalendar::MCInstance();

    switch (component->getType()) {
        case E_EVENT:
            mc->deleteEvent(calendarId, component->getId(), error);
            break;
        case E_TODO:
            mc->deleteTodo(calendarId, component->getId(), error);
            break;
        case E_JOURNAL:
            mc->deleteJournal(calendarId, component->getId(), error);
            break;
    }

    component->setCalendarId(0);
    component->setId(string());
}
