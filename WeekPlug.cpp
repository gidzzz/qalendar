#include "WeekPlug.h"

#include <QGridLayout>
#include <QAction>
#include <QShortcut>

#include "DayWindow.h"
#include "DatePickDialog.h"

#include "WeekArea.h"
#include "WeekdayButton.h"

#include "ChangeManager.h"

using namespace Metrics::WeekWidget;

WeekPlug::WeekPlug(QDate date, QWidget *parent) :
    TemporalPlug(parent),
    date(date)
{
    this->setGlobalDate(date);

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

    connect(weekWidget, SIGNAL(swipedPrev()), this, SLOT(gotoPrevWeek()));
    connect(weekWidget, SIGNAL(swipedNext()), this, SLOT(gotoNextWeek()));

    connect(new QShortcut(QKeySequence(Qt::Key_Left), this), SIGNAL(activated()), this, SLOT(gotoPrevWeek()));
    connect(new QShortcut(QKeySequence(Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(gotoNextWeek()));
}

QString WeekPlug::title() const
{
    int year;
    int week = date.weekNumber(&year);
    return QString("Week %1 (%2)").arg(week).arg(year);
}

void WeekPlug::onActivated()
{
    if (this->isOutdated()
    ||  this->globalDate() != fromGlobalDate(date))
    {
        setDate(this->globalDate()); // Reload
    }

    // This is supposed to refresh the time indicator
    weekWidget->update();
}

void WeekPlug::setDate(QDate date)
{
    this->sync();
    this->setGlobalDate(date);
    // TODO: Set global date only when a date change was explicit (same for other plugs)

    this->date = date;
    weekWidget->setDate(date);

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
    DatePickDialog *dpd = new DatePickDialog(DatePickDialog::Week, date, this);
    if (dpd->exec() == QDialog::Accepted)
        setDate(dpd->date());
    delete dpd;
}

void WeekPlug::gotoPrevWeek()
{
    setDate(date.addDays(-7));
}

void WeekPlug::gotoNextWeek()
{
    setDate(date.addDays(7));
}

void WeekPlug::gotoToday()
{
    if (this->globalDate() == fromGlobalDate(QDate::currentDate())) {
        (new DayWindow(toGlobalDate(date), this))->show();
    } else {
        setDate(QDate::currentDate());
    }
}

void WeekPlug::newEvent()
{
    ChangeManager::newEvent(this, QDateTime(toGlobalDate(date), QTime::currentTime()));
}

QDate WeekPlug::toGlobalDate(QDate date)
{
    QDate currentDate = QDate::currentDate();

    return date.year() == currentDate.year() && date.weekNumber() == currentDate.weekNumber()
         ? currentDate
         : fromGlobalDate(date);
}

QDate WeekPlug::fromGlobalDate(QDate globalDate)
{
    return globalDate.addDays(1 - globalDate.dayOfWeek());
}
