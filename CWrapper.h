#ifndef CWRAPPER_H
#define CWRAPPER_H

#include <algorithm>

#include <CMulticalendar.h>
#include <CCalendar.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>

#include "ComponentInstance.h"

#include <QMetaType>

Q_DECLARE_METATYPE (CEvent*)
Q_DECLARE_METATYPE (CTodo*)
Q_DECLARE_METATYPE (CJournal*)

enum EntryType
{
    E_CALENDAR = 0,
    E_EVENT,
    E_TODO,
    E_JOURNAL,
    E_BDAY,
    E_SPARE,
    E_VTIMEZONE
};

enum RecurrenceType
{
    // This is not in the API, but the API doesn't care that much what value
    // is used for type, so maybe it's not a crime to sneak this in
    E_DISABLED = -1,

    E_NONE = 0, // External
    E_DAILY,
    E_WEEKDAY,
    E_WEEKLY,
    E_MONTHLY,
    E_YEARLY,
    E_COMPLEX
};

enum CalendarColourExtra
{
    COLOUR_NEW = -1
};

namespace CWrapper
{
    inline QString calendarName(const string &name)
    {
        // Handle translatable names
        if (name == "cal_ti_calendar_private")
            return QObject::tr("Private");
        if (name == "cal_ti_smart_birthdays")
            return QObject::tr("Birthdays");

        return QString::fromUtf8(name.c_str());
    }

    inline bool instanceComparator(ComponentInstance *i1, ComponentInstance *i2)
    {
        if (i1->stamp < i2->stamp)
            return true;

        if (i1->stamp == i2->stamp
        &&  i1->duration() > i2->duration())
            return true;

        return false;
    }

    inline bool todoComparator(CTodo *t1, CTodo *t2)
    {
        if (t1->getStatus() < t2->getStatus())
            return true;

        if (t1->getStatus() == t2->getStatus()
        &&  t1->getDue() < t2->getDue())
            return true;

        return false;
    }

    inline bool journalComparator(CJournal *j1, CJournal *j2)
    {
        return j1->getDateStart() > j2->getDateStart();
    }

    inline bool calendarComparator(CCalendar *c1, CCalendar *c2)
    {
        return QString::localeAwareCompare(calendarName(c1->getCalendarName()),
                                           calendarName(c2->getCalendarName())) < 0;
    }

    inline bool calendarVisibilityComparator(CCalendar *c1, CCalendar *c2)
    {
        if (c1->IsShown() && !c2->IsShown())
            return true;

        if (c2->IsShown() && !c1->IsShown())
            return false;

        return calendarComparator(c1, c2);
    }

    inline void sort(vector<ComponentInstance*> &instances)
    {
        sort(instances.begin(), instances.end(), instanceComparator);
    }

    inline void sort(vector<CTodo*> &todos)
    {
        sort(todos.begin(), todos.end(), todoComparator);
    }

    inline void sort(vector<CJournal*> &journals)
    {
       sort(journals.begin(), journals.end(), journalComparator);
    }

    inline void sort(vector<CCalendar*> &calendars, bool visibilityMatters)
    {
        sort(calendars.begin(), calendars.end(), visibilityMatters ? calendarVisibilityComparator
                                                                   : calendarComparator);
    }

    // Expand a component to specific instances
    inline void expand(CComponent *component, vector<ComponentInstance*> &instances, const time_t startStamp, const time_t endStamp)
    {
        const time_t duration = component->getDateEnd() - component->getDateStart();

        if (component->getRecurrence()) {
            vector<time_t> stamps;
            component->getInstanceTimes(startStamp, endStamp, stamps);
            for (unsigned int s = 0; s < stamps.size(); s++) {

                // NOTE: Looks like there is a bug in the backend. When
                // recurrence is present, components are fetched also from
                // outside of the requested range (one day before). Below is
                // a workaround for that.
                if (stamps[s] > endStamp || stamps[s] + duration < startStamp)
                    continue;

                // When calendar-backend is fetching components from a certain
                // range, it treats the component bounds inclusively. However,
                // this is not what we want, so an additional filtering step
                // is required.
                if (stamps[s] + duration == startStamp && duration != 0)
                    continue;

                // NOTE: There is yet another bug in the backend. If multiple
                // recurrence rules with different frequencies are specified and
                // they happen to produce duplicate results, the duplicates will
                // not be removed. We have to find them on our own.
                unsigned int i = 0;
                while (i < instances.size()
                   && (instances[i]->stamp != stamps[s] || instances[i]->component != component))
                       i++;

                // Add the considered instance only if there are no duplicates
                if (i == instances.size())
                    instances.push_back(new ComponentInstance(component, stamps[s]));
            }
        } else {
            // Interpret the end date as non-inclusive (see above)
            if (component->getDateEnd() == startStamp && duration != 0)
                return;

            instances.push_back(new ComponentInstance(component, component->getDateStart()));
        }
    }

    inline void expand(const vector<CComponent*> &components, vector<ComponentInstance*> &instances, const time_t startStamp, const time_t endStamp)
    {
        for (unsigned int i = 0; i < components.size(); i++)
            expand(components[i], instances, startStamp, endStamp);
    }

    // Some functions in the API retrieve incomplete component representations
    // and that can lead to unexpected segfaults. This function returns the full
    // represenation of the provided component.
    inline CEvent* details(CEvent *event)
    {
        int error;
        CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(event->getCalendarId(), error);
        event = calendar->getEvent(event->getId(), error);
        if (event) event->setCalendarId(calendar->getCalendarId()); // It doesn't happen on its own, sigh...
        delete calendar;
        return event;
    }

    inline CTodo* details(CTodo *todo)
    {
        int error;
        CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(todo->getCalendarId(), error);
        todo = calendar->getTodo(todo->getId(), error);
        if (todo) todo->setCalendarId(calendar->getCalendarId());
        delete calendar;
        return todo;
    }

    inline CJournal* details(CJournal *journal)
    {
        int error;
        CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(journal->getCalendarId(), error);
        journal = calendar->getJournal(journal->getId(), error);
        if (journal) journal->setCalendarId(calendar->getCalendarId());
        delete calendar;
        return journal;
    }

    // Adapt the given string to be displayed in one line of a limited length.
    // The purpose of the limit is to prevent wasting too much time
    // on impractical amounts of data that cannot fit on the screen anyway.
    inline QString simplify(const string &std_str, size_t limit)
    {
        return QString::fromUtf8(std_str.c_str(), qMin(std_str.length(), limit)).replace('\n', ' ');
    }

    inline const char* calendarType(const int type)
    {
        switch (type)
        {
            case LOCAL_CALENDAR:
                return "LOCAL_CALENDAR";
            case BIRTHDAY_CALENDAR:
                return "BIRTHDAY_CALENDAR";
            case SYNC_CALENDAR:
                return "SYNC_CALENDAR";
            case DEFAULT_PRIVATE:
                return "DEFAULT_PRIVATE";
            case DEFAULT_SYNC:
                return "DEFAULT_SYNC";
            default:
                return "UNKNOWN";
        }
    }

    inline const char* colorIcon(const int color)
    {
        switch (color) {
            case COLOUR_NEW:
                return "general_add";
            case COLOUR_DARKBLUE:
                return "calendar_colors_darkblue";
            case COLOUR_DARKGREEN:
                return "calendar_colors_darkgreen";
            case COLOUR_DARKRED:
                return "calendar_colors_darkred";
            case COLOUR_ORANGE:
                return "calendar_colors_orange";
            case COLOUR_VIOLET:
                return "calendar_colors_violet";
            case COLOUR_YELLOW:
                return "calendar_colors_yellow";
            case COLOUR_WHITE:
                return "calendar_colors_white";
            case COLOUR_BLUE:
                return "calendar_colors_blue";
            case COLOUR_RED:
                return "calendar_colors_red";
            case COLOUR_GREEN:
                return "calendar_colors_green";
            default:
                return "";
        }
    }

    inline const char* colorStripe(const int color)
    {
        switch (color) {
            case COLOUR_DARKBLUE:
                return "calendar_event_darkblue";
            case COLOUR_DARKGREEN:
                return "calendar_event_darkgreen";
            case COLOUR_DARKRED:
                return "calendar_event_darkred";
            case COLOUR_ORANGE:
                return "calendar_event_orange";
            case COLOUR_VIOLET:
                return "calendar_event_violet";
            case COLOUR_YELLOW:
                return "calendar_event_yellow";
            case COLOUR_WHITE:
                return "calendar_event_white";
            case COLOUR_BLUE:
                return "calendar_event_blue";
            case COLOUR_RED:
                return "calendar_event_red";
            case COLOUR_GREEN:
                return "calendar_event_green";
            default:
                return "";
        }
    }

    inline const char* colorStripeDim(const int color)
    {
        switch (color) {
            case COLOUR_DARKBLUE:
                return "calendar_event_darkblue_dim";
            case COLOUR_DARKGREEN:
                return "calendar_event_darkgreen_dim";
            case COLOUR_DARKRED:
                return "calendar_event_darkred_dim";
            case COLOUR_ORANGE:
                return "calendar_event_orange_dim";
            case COLOUR_VIOLET:
                return "calendar_event_violet_dim";
            case COLOUR_YELLOW:
                return "calendar_event_yellow_dim";
            case COLOUR_WHITE:
                return "calendar_event_white_dim";
            case COLOUR_BLUE:
                return "calendar_event_blue_dim";
            case COLOUR_RED:
                return "calendar_event_red_dim";
            case COLOUR_GREEN:
                return "calendar_event_green_dim";
            default:
                return "";
        }
    }

    inline const char* colorHour(const int color)
    {
        switch (color) {
            case COLOUR_DARKBLUE:
                return "calendar_event_hour_darkblue";
            case COLOUR_DARKGREEN:
                return "calendar_event_hour_darkgreen";
            case COLOUR_DARKRED:
                return "calendar_event_hour_darkred";
            case COLOUR_ORANGE:
                return "calendar_event_hour_orange";
            case COLOUR_VIOLET:
                return "calendar_event_hour_violet";
            case COLOUR_YELLOW:
                return "calendar_event_hour_yellow";
            case COLOUR_WHITE:
                return "calendar_event_hour_white";
            case COLOUR_BLUE:
                return "calendar_event_hour_blue";
            case COLOUR_RED:
                return "calendar_event_hour_red";
            case COLOUR_GREEN:
                return "calendar_event_hour_green";
            default:
                return "";
        }
    }

    inline const char* colorAllDay(const int color)
    {
        switch (color) {
            case COLOUR_DARKBLUE:
                return "calendar_colors_allday_darkblue";
            case COLOUR_DARKGREEN:
                return "calendar_colors_allday_darkgreen";
            case COLOUR_DARKRED:
                return "calendar_colors_allday_darkred";
            case COLOUR_ORANGE:
                return "calendar_colors_allday_orange";
            case COLOUR_VIOLET:
                return "calendar_colors_allday_violet";
            case COLOUR_YELLOW:
                return "calendar_colors_allday_yellow";
            case COLOUR_WHITE:
                return "calendar_colors_allday_white";
            case COLOUR_BLUE:
                return "calendar_colors_allday_blue";
            case COLOUR_RED:
                return "calendar_colors_allday_red";
            case COLOUR_GREEN:
                return "calendar_colors_allday_green";
            default:
                return "";
        }
    }
}

#endif // CWRAPPER_H
