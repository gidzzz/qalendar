#include "WeekPlug.h"

#include <QGridLayout>
#include <QAction>
#include <QShortcut>

#include "DayWindow.h"
#include "DatePickDialog.h"

#include "WeekArea.h"
#include "WeekdayButton.h"

#include "ChangeManager.h"
#include "Date.h"

using namespace Metrics::WeekWidget;

WeekPlug::WeekPlug(QDate date, QWidget *parent) :
    TemporalPlug(parent)
{
    this->setGlobalDate(date);
    this->date = fromGlobalDate(date);

    QGridLayout *mainLayout = new QGridLayout(this);
    weekdayLayout = new QHBoxLayout();
    weekdayLayout->setContentsMargins(TimeWidth, 0, 0, 0);

    WeekArea *weekArea = new WeekArea(this);
    weekWidget = weekArea->widget();

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Add weekday buttons
    for (int i = 0; i < NumWeekdays; i++)
        weekdayLayout->addWidget(new WeekdayButton(this));
    weekdayLayout->addStretch();

    mainLayout->addLayout(weekdayLayout, 0, 0);
    mainLayout->addWidget(weekArea, 1, 0);

    // Set up menu actions
    QAction *prevAction = new QAction(tr("Previous week"), this);
    QAction *nextAction = new QAction(tr("Next week"), this);
    QAction *todayAction = new QAction(tr("Jump to today"), this);
    QAction *jumpAction = new QAction(tr("Jump to"), this);
    QAction *eventAction = new QAction(tr("New event"), this);
    actions.append(prevAction);
    actions.append(nextAction);
    actions.append(todayAction);
    actions.append(jumpAction);
    actions.append(eventAction);
    connect(prevAction, SIGNAL(triggered()), this, SLOT(gotoPrevWeek()));
    connect(nextAction, SIGNAL(triggered()), this, SLOT(gotoNextWeek()));
    connect(todayAction, SIGNAL(triggered()), this, SLOT(gotoToday()));
    connect(jumpAction, SIGNAL(triggered()), this, SLOT(selectWeek()));
    connect(eventAction, SIGNAL(triggered()), this, SLOT(newEvent()));

    connect(weekWidget, SIGNAL(swipedRight()), this, SLOT(gotoPrevWeek()));
    connect(weekWidget, SIGNAL(swipedLeft()), this, SLOT(gotoNextWeek()));

    connect(new QShortcut(QKeySequence(Qt::Key_Left), this), SIGNAL(activated()), this, SLOT(gotoPrevWeek()));
    connect(new QShortcut(QKeySequence(Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(gotoNextWeek()));
}

QString WeekPlug::title() const
{
    int year;
    int week = Date::relWeekNumber(this->date, &year);
    return QString(tr("Week %1 (%2)")).arg(week).arg(year);
}

void WeekPlug::onActivated()
{
    // This is supposed to refresh the time indicator
    weekWidget->update();

    Plug::onActivated();
}

void WeekPlug::onChange()
{
    setDate(currentDateLock ? QDate::currentDate() : this->globalDate());
}

void WeekPlug::setDate(QDate date)
{
    this->sync();

    this->setGlobalDate(date);
    this->date = fromGlobalDate(date);
    // TODO: Set global date only when a date change was explicit (same for other plugs)

    currentDateLock = this->date == fromGlobalDate(QDate::currentDate());

    weekWidget->setDate(this->date);

    QDate weekdayProbe = weekWidget->firstDate();
    for (int i = 0; i < weekdayLayout->count()-1; i++) {
        WeekdayButton *button = qobject_cast<WeekdayButton*>(weekdayLayout->itemAt(i)->widget());
        button->setDate(weekdayProbe);
        weekdayProbe = weekdayProbe.addDays(1);
    }

    emit titleChanged(title());
}

void WeekPlug::selectWeek()
{
    DatePickDialog *dpd = new DatePickDialog(DatePickDialog::Week, this->date, this);
    if (dpd->exec() == QDialog::Accepted)
        setDate(dpd->date());
    delete dpd;
}

void WeekPlug::gotoPrevWeek()
{
    setDate(this->date.addDays(-7));
}

void WeekPlug::gotoNextWeek()
{
    setDate(this->date.addDays(7));
}

void WeekPlug::gotoToday()
{
    if (this->globalDate() == fromGlobalDate(QDate::currentDate())) {
        (new DayWindow(toGlobalDate(this->date), this))->show();
    } else {
        setDate(QDate::currentDate());
    }
}

void WeekPlug::newEvent()
{
    ChangeManager::newEvent(this, QDateTime(toGlobalDate(this->date), QTime::currentTime()));
}

QDate WeekPlug::toGlobalDate(QDate date)
{
    QDate currentDate = QDate::currentDate();

    int year, currentYear;

    return Date::relWeekNumber(date, &year) == Date::relWeekNumber(currentDate, &currentYear) && year == currentYear
         ? currentDate
         : fromGlobalDate(date);
}

QDate WeekPlug::fromGlobalDate(QDate globalDate)
{
    return globalDate.addDays(1 - Date::relDayOfWeek(globalDate.dayOfWeek()));
}
