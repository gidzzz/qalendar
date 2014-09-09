#include "BirthdayCalendar.h"

#include <QDebug>

#include <CAlarm.h>
#include <CMulticalendar.h>
#include <CRecurrence.h>
#include "CWrapper.h"

#include "ChangeManager.h"

// NOTE: Current implementation does not make any use of e_book_get_changes(),
// probably leaving some room for improvements in startup time. However, current
// approach is not necessarily worse, because it does not leave any junk in
// ~/.osso-abook/db/*.changes.db and the complete check at each startup might
// prove itself more reliable in some extreme cases. Moreover, startup time is
// not as important here, because the birthday calendar is enabled only when the
// background mode is enabled, so the application is initially hidden and the
// startup time cannot be easily observed.

BirthdayCalendar::BirthdayCalendar(CCalendar *calendar) :
    calendar(calendar),
    bookView(NULL),
    initialized(false)
{
    // Obtain the book
    book = e_book_new_system_addressbook(NULL);
    if (!book) return;

    // Open the book to allow further operations
    if (!e_book_open(book, false, NULL)) return;

    // Obtain the view of birthday-enabed contacts
    EBookQuery *query = e_book_query_vcard_field_exists("BDAY");
    e_book_get_book_view(book, query, NULL, 0, &bookView, NULL);
    e_book_query_unref(query);
    if (!bookView) return;

    // Connect signals
    g_signal_connect(bookView, "contacts-added"   , G_CALLBACK(&onContactsAdded   ), static_cast<void*>(this));
    g_signal_connect(bookView, "contacts-changed" , G_CALLBACK(&onContactsChanged ), static_cast<void*>(this));
    g_signal_connect(bookView, "contacts-removed" , G_CALLBACK(&onContactsRemoved ), static_cast<void*>(this));
    g_signal_connect(bookView, "sequence-complete", G_CALLBACK(&onSequenceComplete), static_cast<void*>(this));

    // Start watching the view
    e_book_view_start(bookView);
}

BirthdayCalendar::~BirthdayCalendar()
{
    if (book) {
        if (bookView) {
            e_book_view_stop(bookView);
            g_signal_handlers_disconnect_matched(bookView, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, this);
            g_object_unref(bookView);
        }
        g_object_unref(book);
    }
    delete calendar;
}

int BirthdayCalendar::id() const
{
    return calendar->getCalendarId();
}

// Load given event's alarm settings from the database and apply them to the instance
void BirthdayCalendar::loadAlarm(CBdayEvent *event)
{
    int error;

    if (CEvent *storedEvent = calendar->getBirthDayEvent(event->getId(), error))
        event->setAlarm(storedEvent->getAlarm());
}

// Convert EContact to CBdayEvent
CBdayEvent* BirthdayCalendar::toBdayEvent(EContact *contact)
{
    // Extract the birth date
    const EContactDate *bday = static_cast<const EContactDate*>(e_contact_get_const(contact, E_CONTACT_BIRTH_DATE));
    int date = QDateTime(QDate(qBound(1972u, bday->year, 2036u), bday->month, bday->day)).toTime_t();
    // NOTE: The stock calendar changes the birthdays of people who were born on
    // the 29th of February in 1968 or an earlier year to the 1st of March. This
    // program takes a different approach, by anchoring all birthday events
    // between 1972 and 2036. The late side of the range is also different from
    // the stock calendar, as it appears to be using the current year instead of
    // a distant one.
    // NOTE: There seems to be a bug in calendar-backend, preventing birthdays
    // of February 29 from appearing in the single day view for March 1
    // in non-leap years.

    // Prepare a yearly recurrence
    CRecurrence recurrence;
    recurrence.setRrule(vector<string>(1, "FREQ=YEARLY"));
    recurrence.setRtype(E_COMPLEX); // UNTIL part is missing

    // E_CONTACT_NAME_OR_ORG ignores nicknames, chack manually when appropriate
    const char *summary = static_cast<const char*>(e_contact_get_const(contact, E_CONTACT_NAME_OR_ORG));
    if (!summary)
        summary = static_cast<const char*>(e_contact_get_const(contact, E_CONTACT_NICKNAME));

    // Create an event and fill the necessary fields
    CBdayEvent *event = new CBdayEvent();
    event->setType(E_BDAY);
    event->setId(static_cast<const char*>(e_contact_get_const(contact, E_CONTACT_UID)));
    event->setSummary(summary ? summary : "CONTACT-" + event->getId());
    event->setDateStart(date);
    event->setDateEnd(date);
    event->setAllDay(true);
    event->setRecurrence(&recurrence);
    event->setStatus(bday->year); // Mimic the stock calendar by storing the year here

    return event;
}

// Handle new contacts
void BirthdayCalendar::onContactsAdded(EBookView *, GList *vcards, BirthdayCalendar *self)
{
    // Check whether this is a real change or just initialization
    if (self->initialized) {
        // Convert contacts to events
        vector<CBdayEvent*> eventsToAdd;
        for (; vcards; vcards = vcards->next)
            eventsToAdd.push_back(toBdayEvent(static_cast<EContact*>(vcards->data)));

        // Add the events
        int error;
        CMulticalendar::MCInstance()->addBirthdays(eventsToAdd, error);
        ChangeManager::bump();

        // Clean up
        for (unsigned int i = 0; i < eventsToAdd.size(); i++)
            delete eventsToAdd[i];
    } else {
        // Convert contacts to events and wait for the sequence to complete
        for (; vcards; vcards = vcards->next)
            self->eventBuffer.push_back(toBdayEvent(static_cast<EContact*>(vcards->data)));
    }
}

// Handle modifications made to contacts
void BirthdayCalendar::onContactsChanged(EBookView *, GList *vcards, BirthdayCalendar *self)
{
    for (; vcards; vcards = vcards->next) {
        int error;

        CBdayEvent *event = toBdayEvent(static_cast<EContact*>(vcards->data));
        event->setId(self->calendar->getExternalToLocalId(event->getId(), true, error));
        self->loadAlarm(event);

        CMulticalendar::MCInstance()->modifyBirthDay(event, error);

        delete event;
    }
    ChangeManager::bump();
}

// Handle removal of contacts
void BirthdayCalendar::onContactsRemoved(EBookView *, GList *ids, BirthdayCalendar *)
{
    // Convert the list into a vector
    vector<string> idsToDelete;
    for (; ids; ids = ids->next)
        idsToDelete.push_back(static_cast<const char*>(ids->data));

    // Remove the events
    int error;
    CMulticalendar::MCInstance()->deleteBirthdays(idsToDelete, error);
    ChangeManager::bump();
}

// Bring birthdays up to date when the initial sequence of contacts is complete
void BirthdayCalendar::onSequenceComplete(EBookView *, gint, BirthdayCalendar *self)
{
    if (self->initialized) return;

    int bump = false;
    int error;

    // Get all birthdays events currently in the calendar
    vector<CComponent*> components = self->calendar->getComponents(E_BDAY, -1, -1, error);

    // Find the events which should no longer exist
    vector<string> idsToDelete;
    for (unsigned int i = components.size(); i > 0;) {
        i--;

        // Obtain the contact ID
        string externalId = self->calendar->getExternalToLocalId(components[i]->getId(), false, error);

        if (externalId.empty()) {
            // NOTE: Apparently the API does not provide any way to remove
            // a birthday event which does not have a mapping to a contact. Such
            // situation is possible because the API does not take into account
            // the possibility of multiple birthday calendars containing entries
            // for the same contact and stores up to one mapping per contact.
            qDebug() << "Oops, an unassociated birthday!";

            delete components[i];
            components.erase(components.begin() + i);
            continue;
        }

        // Check if the associated contact still exists
        bool deleted = true;
        for (unsigned int i = 0; i < self->eventBuffer.size(); i++) {
            if (externalId == self->eventBuffer[i]->getId()) {
                deleted = false;
                break;
            }
        }

        if (deleted) {
            delete components[i];
            components.erase(components.begin() + i);
            idsToDelete.push_back(externalId);
        } else {
            components[i]->setId(externalId);
        }
    }

    // Remove the marked events
    if (!idsToDelete.empty()) {
        CMulticalendar::MCInstance()->deleteBirthdays(idsToDelete, error);
        bump = true;
    }

    // Add new events and update old ones
    vector<CBdayEvent*> eventsToAdd;
    for (unsigned int i = 0; i < self->eventBuffer.size(); i++) {
        CBdayEvent *event = self->eventBuffer[i];

        // Obtain local ID mapping for the contact
        string localId = self->calendar->getExternalToLocalId(event->getId(), true, error);

        // Differentiate between new and existing events
        if (localId.empty()) {
            // The event is new
            eventsToAdd.push_back(event);
        } else {
            // Find the matching event from the calendar
            for (unsigned int i = 0; i < components.size(); i++) {
                if (components[i]->getId() == event->getId()) {
                    // Recognize outdated events
                    if (components[i]->getSummary()   != event->getSummary()
                    ||  components[i]->getDateStart() != event->getDateStart()
                    ||  components[i]->getDateEnd()   != event->getDateEnd()
                    ||  components[i]->getAllDay()    != event->getAllDay()
                    ||  components[i]->getStatus()    != event->getStatus())
                    // TODO: Compare the recurrences? How much freedom should be allowed?
                    {
                        // Update the event
                        event->setId(localId);
                        self->loadAlarm(event);
                        CMulticalendar::MCInstance()->modifyBirthDay(event, error);
                        bump = true;
                    }
                    break;
                }
            }
        }
    }

    // Add new events
    if (!eventsToAdd.empty()) {
        CMulticalendar::MCInstance()->addBirthdays(eventsToAdd, error);
        bump = true;
    }

    for (unsigned int i = 0; i < components.size(); i++)
        delete components[i];

    for (unsigned int i = 0; i < self->eventBuffer.size(); i++)
        delete self->eventBuffer[i];

    vector<CBdayEvent*>().swap(self->eventBuffer);

    self->initialized = true;

    if (bump)
        ChangeManager::bump();
}
