#include "DatePickDialog.h"

#include "ExpandingListWidget.h"
#include "DateDayDelegate.h"

#include "Date.h"
#include "Roles.h"

const int minYear = 1970; // WARNING: This might be too low for some time zones
const int maxYear = 2037;
// TODO: Enforce date limits in other widgets

// TODO: Fix premature item activation

DatePickDialog::DatePickDialog(Type type, QDate date, QWidget *parent) :
    DateTimePickDialog(parent),
    type(type)
{
    setWindowTitle(tr("Select date"));

    // Make sure that the provided date is in the supported range
    date = qBound(QDate(minYear, 1, 1), date, QDate(maxYear, 12, 31));

    // Create the day list
    if (type == Day) {
        lists[Day] = new ExpandingListWidget(this);
        lists[Day]->setItemDelegate(new DateDayDelegate(lists[Day]));
        ui->listLayout->addWidget(lists[Day]);
        scrollers.append(Scroller(lists[Day], 2, 1));
        connect(lists[Day], SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(centerView()));
    }
    // Create the week list
    if (type == Week) {
        lists[Week] = new ExpandingListWidget(this);
        ui->listLayout->addWidget(lists[Week]);
        scrollers.append(Scroller(lists[Week], 2, 1));
        connect(lists[Week], SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(centerView()));
    }
    // Create the month list
    if (type == Month || type == Day) {
        lists[Month] = new ExpandingListWidget(this);
        for (int m = 1; m <= 12; m++)
            lists[Month]->addItem(QLocale().standaloneMonthName(m));
        ui->listLayout->addWidget(lists[Month]);
        scrollers.append(Scroller(lists[Month], 2, 1));
        connect(lists[Month], SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(centerView()));
    }
    // Create the year list
    lists[Year] = new ExpandingListWidget(this);
    for (int y = minYear; y <= maxYear; y++)
        lists[Year]->addItem(QString::number(y));
    ui->listLayout->addWidget(lists[Year]);
    scrollers.append(Scroller(lists[Year], 4, minYear));
    connect(lists[Year], SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(centerView()));

    // Set up the year list
    if (type != Week) {
        lists[Year]->setCurrentRow(date.year()-minYear);
    }
    // Set up the month list
    if (type == Month || type == Day) {
        lists[Month]->setCurrentRow(date.month()-1);
    }
    // Set up the week list
    if (type == Week) {
        int year;
        int week = Date::relWeekNumber(date, &year);

        lists[Year]->setCurrentRow(year-minYear);

        adjustWeeks();
        lists[Week]->setCurrentRow(week-1);

        connect(lists[Year], SIGNAL(itemSelectionChanged()), this, SLOT(adjustWeeks()));
    }
    // Set up the day list
    if (type == Day) {
        adjustDays();
        lists[Day]->setCurrentRow(date.day()-1);
        connect(lists[Month], SIGNAL(itemSelectionChanged()), this, SLOT(adjustDays()));
        connect(lists[Year],  SIGNAL(itemSelectionChanged()), this, SLOT(adjustDays()));
    }
}

void DatePickDialog::centerView()
{
    // Scroll day list
    if (type == Day)
        centerView(lists[Day]);

    // Scroll week list
    if (type == Week)
        centerView(lists[Week]);

    // Scroll month list
    if (type == Month || type == Day)
        centerView(lists[Month]);

    // Scroll year list
    centerView(lists[Year]);
}

void DatePickDialog::adjustDays()
{
    const int targetDays = QDate(year(), month(), 1).daysInMonth();
    const int initialDays = lists[Day]->count();

    if (initialDays < targetDays) {
        for (int i = initialDays; i < targetDays; i++)
            lists[Day]->addItem(QString::number(i+1));
    } else if (initialDays > targetDays) {
        bool reselect = row(lists[Day]) >= targetDays;

        for (int i = initialDays; i > targetDays; i--)
            delete lists[Day]->item(i-1);

        if (reselect)
            lists[Day]->setCurrentRow(targetDays-1);
    }

    for (int i = 0; i < lists[Day]->count(); i++)
        lists[Day]->item(i)->setData(DateRole, QDate(year(), month(), i+1));
}

void DatePickDialog::adjustWeeks()
{
    const int targetWeeks = QDate(year(), 12, 28).weekNumber(); // This day is always in the last week of the year
    const int initialWeeks = lists[Week]->count();

    if (initialWeeks < targetWeeks) {
        for (int i = initialWeeks; i < targetWeeks; i++)
            lists[Week]->addItem(QString::number(i+1));
    } else if (initialWeeks > targetWeeks) {
        bool reselect = row(lists[Week]) >= targetWeeks;

        for (int i = initialWeeks; i > targetWeeks; i--)
            delete lists[Week]->item(i-1);

        if (reselect)
            lists[Week]->setCurrentRow(targetWeeks-1);
    }

    // Find the first day of the first week
    QDate weekProbe(year(), 1, 1);
    const int weekday = Date::relDayOfWeek(weekProbe.dayOfWeek());
    weekProbe = weekProbe.addDays(weekday > 4 ? (1 + 7-weekday)
                                              : (1 - weekday));

    for (int i = 0; i < targetWeeks; i++) {
        lists[Week]->item(i)->setText(QString(tr("Week %1 (%2 - %3)"))
                                     .arg(i+1)
                                     .arg(Date::toString(weekProbe, Date::Partial))
                                     .arg(Date::toString(weekProbe.addDays(6), Date::Partial)));
        weekProbe = weekProbe.addDays(7);
    }
}

int DatePickDialog::day()
{
    return row(lists[Day]) + 1;
}

int DatePickDialog::month()
{
    return row(lists[Month]) + 1;
}

int DatePickDialog::year()
{
    return row(lists[Year]) + minYear;
}

QDate DatePickDialog::date()
{
    switch (type) {
        case Day:
            return QDate(year(), month(), day());
        case Week:
            return QDate(year(), 1, 4).addDays(row(lists[Week])*7);
        case Month:
            return QDate(year(), month(), 1);
        case Year:
            return QDate(year(), 1, 1);
        default:
            return QDate();
    }
}

void DatePickDialog::accept()
{
    emit selected(date());

    DateTimePickDialog::accept();
}
