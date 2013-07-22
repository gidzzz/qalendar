#ifndef CHANGEMANAGER_H
#define CHANGEMANAGER_H

#include <QWidget>

#include <QDateTime>
#include <QDate>

#include <CCalendar.h>
#include <CComponent.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>

typedef unsigned int Version;

class ChangeManager
{
public:
    static Version version();
    static void bump();

    static bool edit(QWidget *parent, CComponent *component);

    static bool drop(QWidget *parent, CComponent *component);

    static bool newEvent(QWidget *parent);
    static bool newEvent(QWidget *parent, QDateTime from);
    static bool newEvent(QWidget *parent, QDateTime from, QDateTime to, bool allDay);

    static bool newTodo(QWidget *parent);

    static bool newJournal(QWidget *parent);

    static bool save(CComponent *component, int calendarId = -1);

private:
    static Version m_version;

    static void addComponent(CCalendar* calendar, CComponent *component, int &error);
    static void modifyComponent(CCalendar* calendar, CComponent *component, int &error);
    static void deleteComponent(CCalendar *calendar, CComponent *component, int &error);
};

#endif // CHANGEMANAGER_H
