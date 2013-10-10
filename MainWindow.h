#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMenu>

#include <QDate>

#include <CMulticalendar.h>
#include <CCalendar.h>

#include "MonthPlug.h"
#include "WeekPlug.h"
#include "AgendaPlug.h"
#include "TodosPlug.h"
#include "JournalsPlug.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    #define DBUS_SERVICE "com.nokia.calendar"
    #define DBUS_PATH "/com/nokia/calendar"
    #define DBUS_INTERFACE "com.nokia.calendar"
    Q_CLASSINFO("D-Bus Interface", "com.nokia.calendar")

public:
    MainWindow(bool runInBackground);

    void setPlug(Plug *plug);

public slots:
    Q_SCRIPTABLE void top_application();
    Q_SCRIPTABLE void launch_view(uint type, int, QString componentId, int calendarId = 0);
    Q_SCRIPTABLE void deferred_close();

private:
    QMenu *windowMenu;

    MonthPlug *monthPlug;
    WeekPlug *weekPlug;
    AgendaPlug *agendaPlug;
    TodosPlug *todosPlug;
    JournalsPlug *journalsPlug;

    Plug *currentPlug;

    QAction *monthAction;
    QAction *weekAction;
    QAction *agendaAction;
    QAction *todosAction;
    QAction *journalsAction;
    QAction *settingsAction;

    QActionGroup *plugGroup;

    QLayout *mainLayout;

    bool runInBackground;

    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *e);

    void deleteOldComponents();

    QMainWindow *topWindow();

    void showComponent(CEvent *event, QMainWindow *parent);
    void showComponent(CTodo *todo, QMainWindow *parent);

    // Display component C obtained using function Getter
    template<class C, C* (CCalendar::*Getter)(string, int&)>
    void showComponent(int calendarId, QString componentId)
    {
        // Check if there is a window which could be used as a parent
        if (QMainWindow *topWindow = this->topWindow()) {
            int error;

            // Get the calendar
            if (CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(calendarId, error)) {
                // Get the component
                if (C *component = (calendar->*Getter)(componentId.toAscii().data(), error)) {
                    // Fix up and display the component
                    component->setCalendarId(calendarId);
                    showComponent(component, topWindow);
                    delete component;
                }
                delete calendar;
            }
        }
    }

private slots:
    void showMonth();
    void showWeek(QDate date = QDate());
    void showAgenda();
    void showTodos();
    void showJournals();

    void openSettings();
};

#endif // MAINWINDOW_H
