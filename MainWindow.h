#include <QMainWindow>

#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>

#include <QDate>

#include <QSettings>

#include <calendar-backend/CMulticalendar.h>

#include "MonthPlug.h"
#include "WeekPlug.h"
#include "AgendaPlug.h"
#include "TodosPlug.h"
#include "JournalsPlug.h"

#include "SettingsDialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    void setPlug(Plug *plug);

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

    void changeEvent(QEvent *e);

    void deleteOldComponents();

private slots:
    void showMonth();
    void showWeek(QDate date = QDate());
    void showAgenda();
    void showTodos();
    void showJournals();

    void openSettings();
};
