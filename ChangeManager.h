#ifndef CHANGEMANAGER_H
#define CHANGEMANAGER_H

#include <QWidget>

#include <QTimer>
#include <QDateTime>
#include <QDate>

#include <CCalendar.h>
#include <CComponent.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>

typedef unsigned int Version;

class ChangeClient;

class ChangeManager : public QObject
{
    Q_OBJECT

public:
    static Version version();
    static void bump();

    static void activateClient(ChangeClient *client);
    static void deactivateClient(ChangeClient *client);

    static bool edit(QWidget *parent, CComponent *component);
    static bool drop(QWidget *parent, CComponent *component);

    static bool newEvent(QWidget *parent);
    static bool newEvent(QWidget *parent, QDateTime from);
    static bool newEvent(QWidget *parent, QDateTime from, QDateTime to, bool allDay);

    static bool newTodo(QWidget *parent);
    static bool newTodo(QWidget *parent, QDate due);

    static bool newJournal(QWidget *parent);

    static bool save(CComponent *component, int calendarId = -1);

private:
    ChangeManager();

    static ChangeManager *instance;

    static Version m_version;
    static QDate m_date;

    static ChangeClient *activeClient;
    static QTimer *dateCheckTimer;

    static void addComponent(CComponent *component, int calendarId, int &error);
    static void modifyComponent(CComponent *component, int calendarId, int &error);
    static void deleteComponent(CComponent *component, int calendarId, int &error);

private slots:
    static void checkDate();

    static void onDbChange(QString details, QString appName);
};

#endif // CHANGEMANAGER_H
