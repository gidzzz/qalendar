#include "MonthPlug.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QAction>
#include <QPainter>
#include <QShortcut>
#include <QLabel>

#include "DayWindow.h"
#include "DatePickDialog.h"

#include "WeekButton.h"

#include "ChangeManager.h"

using namespace Metrics::MonthWidget;

MonthPlug::MonthPlug(QDate date, QWidget *parent) :
    TemporalPlug(parent),
    date(date)
{
    this->setGlobalDate(date);

    QGridLayout *mainLayout = new QGridLayout(this);
    QHBoxLayout *dayLayout  = new QHBoxLayout();
    weekLayout = new QVBoxLayout();

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    monthWidget = new MonthWidget(QDate(), this);

    // Add week buttons
    for (int i = 0; i < NumWeeks; i++) {
        WeekButton *weekButton = new WeekButton(this);
        connect(weekButton, SIGNAL(clicked(QDate)), this, SLOT(onWeekClicked(QDate)));
        weekLayout->addWidget(weekButton);
    }

    // Add weekday labels
    QFont font = this->font();
    font.setPointSize(13);
    font.setBold(true);
    for (int i = 0; i < NumWeekdays; i++) {
        QLabel *dayLabel = new QLabel(QDate::shortDayName(i+1));
        dayLabel->setAlignment(Qt::AlignHCenter);
        dayLabel->setFont(font);
        dayLayout->addWidget(dayLabel);
    }

    mainLayout->addLayout(dayLayout, 0, 1);
    mainLayout->addLayout(weekLayout, 1, 0);
    mainLayout->addWidget(monthWidget, 1, 1);

    // Se up menu actions
    QAction *prevAction = new QAction(tr("Previous month"), this);
    QAction *nextAction = new QAction(tr("Next month"), this);
    QAction *todayAction = new QAction(tr("Jump to today"), this);
    QAction *jumpAction = new QAction(tr("Jump to"), this);
    QAction *eventAction = new QAction(tr("New event"), this);
    actions.append(prevAction);
    actions.append(nextAction);
    actions.append(todayAction);
    actions.append(jumpAction);
    actions.append(eventAction);
    connect(prevAction, SIGNAL(triggered()), this, SLOT(gotoPrevMonth()));
    connect(nextAction, SIGNAL(triggered()), this, SLOT(gotoNextMonth()));
    connect(todayAction, SIGNAL(triggered()), this, SLOT(gotoToday()));
    connect(jumpAction, SIGNAL(triggered()), this, SLOT(selectMonth()));
    connect(eventAction, SIGNAL(triggered()), this, SLOT(newEvent()));

    connect(monthWidget, SIGNAL(swipedPrev()), this, SLOT(gotoPrevMonth()));
    connect(monthWidget, SIGNAL(swipedNext()), this, SLOT(gotoNextMonth()));
    connect(monthWidget, SIGNAL(swipedPrevFar()), this, SLOT(gotoPrevYear()));
    connect(monthWidget, SIGNAL(swipedNextFar()), this, SLOT(gotoNextYear()));

    connect(new QShortcut(QKeySequence(Qt::Key_Up), this), SIGNAL(activated()), this, SLOT(gotoPrevMonth()));
    connect(new QShortcut(QKeySequence(Qt::Key_Down), this), SIGNAL(activated()), this, SLOT(gotoNextMonth()));
    connect(new QShortcut(QKeySequence(Qt::Key_Left), this), SIGNAL(activated()), this, SLOT(gotoPrevYear()));
    connect(new QShortcut(QKeySequence(Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(gotoNextYear()));
}

QString MonthPlug::title() const
{
    return date.toString("MMMM yyyy");
}

void MonthPlug::onActivated()
{
    if (this->isOutdated()
    ||  this->globalDate() != fromGlobalDate(date))
    {
        setDate(this->globalDate()); // Reload
    }
}

void MonthPlug::setDate(QDate date)
{
    this->sync();
    this->setGlobalDate(date);

    monthWidget->setDate(date);

    // Update week buttons
    QDate weekProbe = monthWidget->firstDate();
    for (int i = 0; i < weekLayout->count(); i++) {
        WeekButton *button = qobject_cast<WeekButton*>(weekLayout->itemAt(i)->widget());
        button->setDate(weekProbe);
        weekProbe = weekProbe.addDays(7);
    }

    // Set the appropriate background
    const char *months[] = { "jan", "feb", "mar", "apr", "may", "jun",
                             "jul", "aug", "sep", "oct", "nov", "dec" };
    QString filename = QString("/etc/hildon/theme/calendar/qgn_calendar_bg_%1.jpg")
                       .arg(months[date.month()-1]);
    background = QPixmap(filename);
    // Looks like the format of png files with jpg extension cannot be detected, try manually
    if (background.isNull())
        background = QPixmap(filename, "png");

    this->update();

    emit titleChanged(title());
}

void MonthPlug::selectMonth()
{
    DatePickDialog *dpd = new DatePickDialog(DatePickDialog::Month, date, this);
    if (dpd->exec() == QDialog::Accepted)
        setDate(dpd->date());
    delete dpd;
}

void MonthPlug::gotoPrevMonth()
{
    setDate(date.addMonths(-1));
}

void MonthPlug::gotoNextMonth()
{
    setDate(date.addMonths(1));
}

void MonthPlug::gotoPrevYear()
{
    setDate(date.addYears(-1));
}

void MonthPlug::gotoNextYear()
{
    setDate(date.addYears(1));
}

void MonthPlug::gotoToday()
{
    if (this->globalDate() == fromGlobalDate(QDate::currentDate())) {
        (new DayWindow(toGlobalDate(date), this))->show();
    } else {
        setDate(QDate::currentDate());
    }
}

void MonthPlug::newEvent()
{
    ChangeManager::newEvent(this, QDateTime(toGlobalDate(date), QTime::currentTime()));
}

void MonthPlug::paintEvent(QPaintEvent *)
{
    QPainter(this).drawPixmap(0, 0, background);
}

void MonthPlug::onWeekClicked(QDate date)
{
    emit weekActivated(date);
}

QDate MonthPlug::toGlobalDate(QDate date)
{
    QDate currentDate = QDate::currentDate();

        this->date = QDate(date.year(), date.month(), qMin(date.daysInMonth(), QDate::currentDate().day()));

    return date.year() == currentDate.year() && date.month() == currentDate.month()
         ? currentDate
         : fromGlobalDate(date);
}

QDate MonthPlug::fromGlobalDate(QDate globalDate)
{
    return globalDate.addDays(1 - globalDate.day());;
}
