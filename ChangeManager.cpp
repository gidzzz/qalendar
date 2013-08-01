#include "ChangeManager.h"

#include <QMessageBox>
#include <QMaemo5InformationBox>

#include <CalendarErrors.h>

#include "EventEditDialog.h"
#include "TodoEditDialog.h"
#include "JournalEditDialog.h"

#include "CWrapper.h"

Version ChangeManager::m_version = 1;

// Return the version of tracked database state
Version ChangeManager::version()
{
    return m_version;
}

// Bump the database state version
void ChangeManager::bump()
{
    m_version++;
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
            // TODO: Editable birthdays?
            QMaemo5InformationBox::information(parent, QObject::tr("Unable to edit birthdays"));
            break;
    }

    return ed && ed->exec() == QDialog::Accepted;
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

    CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(component->getCalendarId(), error);

    if (error == CALENDAR_OPERATION_SUCCESSFUL) {
        switch (component->getType()) {
            case E_EVENT:
                calendar->deleteEvent(component->getId(), error);
                break;
            case E_TODO:
                calendar->deleteTodo(component->getId(), error);
                break;
            case E_JOURNAL:
                calendar->deleteJournal(component->getId(), error);
                break;
        }
    }

    delete calendar;

    bump();

    return error == CALENDAR_OPERATION_SUCCESSFUL;
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
    if (component->getCalendarId()) {
        CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(component->getCalendarId(), error);
        if (calendarId == component->getCalendarId()) {
            // Simply save chages
            modifyComponent(calendar, component, error);
        } else {
            // Move the component between calendars, start by removing it from the old one
            deleteComponent(calendar, component, error);
        }
        delete calendar;
    }

    // Add the component to a calendar if it is a fresh one or has to be moved
    if (!component->getCalendarId()) {
        CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(calendarId, error);
        addComponent(calendar, component, error);
        delete calendar;
    }

    bump();

    // TODO: Error handling, both here and in edit dialogs
    return true;
}

// Add the component to the calendar (helper for the low-level save)
void ChangeManager::addComponent(CCalendar* calendar, CComponent *component, int &error)
{
    component->setCalendarId(calendar->getCalendarId());

    switch (component->getType()) {
        case E_EVENT:
            calendar->addEvent(static_cast<CEvent*>(component), error);
            break;
        case E_TODO:
            calendar->addTodo(static_cast<CTodo*>(component), error);
            break;
        case E_JOURNAL:
            calendar->addJournal(static_cast<CJournal*>(component), error);
            break;
        // TODO: Set error if the type cannot be handled?
    }
}

// Modify the component belonging the calendar (helper for the low-level save)
void ChangeManager::modifyComponent(CCalendar* calendar, CComponent *component, int &error)
{
    switch (component->getType()) {
        case E_EVENT:
            calendar->modifyEvent(static_cast<CEvent*>(component), error);
            break;
        case E_TODO:
            calendar->modifyTodo(static_cast<CTodo*>(component), error);
            break;
        case E_JOURNAL:
            calendar->modifyJournal(static_cast<CJournal*>(component), error);
            break;
    }
}

// Delete the component from the calendar (helper for the low-level save)
void ChangeManager::deleteComponent(CCalendar *calendar, CComponent *component, int &error)
{
    switch (component->getType()) {
        case E_EVENT:
            calendar->deleteEvent(component->getId(), error);
            break;
        case E_TODO:
            calendar->deleteTodo(component->getId(), error);
            break;
        case E_JOURNAL:
            calendar->deleteJournal(component->getId(), error);
            break;
    }

    component->setCalendarId(0);
}
