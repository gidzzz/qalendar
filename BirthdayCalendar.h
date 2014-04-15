#ifndef BIRTHDAYCALENDAR
#define BIRTHDAYCALENDAR

#include <CCalendar.h>
#include <CBdayEvent.h>

#include <libebook/e-book.h>

class BirthdayCalendar
{
public:
    BirthdayCalendar(CCalendar *calendar);
    ~BirthdayCalendar();

    int id() const;

private:
    CCalendar *calendar;
    EBook *book;
    EBookView *bookView;

    bool initialized;
    vector<CBdayEvent*> eventBuffer;

    void loadAlarm(CBdayEvent *event);

    static CBdayEvent* toBdayEvent(EContact *contact);

    static void onContactsAdded   (EBookView *, GList *vcards, BirthdayCalendar *self);
    static void onContactsChanged (EBookView *, GList *vcards, BirthdayCalendar *self);
    static void onContactsRemoved (EBookView *, GList *ids   , BirthdayCalendar *);
    static void onSequenceComplete(EBookView *, gint         , BirthdayCalendar *self);
};

#endif // BIRTHDAYCALENDAR
