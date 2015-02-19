#include "DatePickDialog.h"

#include "ExpandingListWidget.h"
#include "DateDayDelegate.h"

#include "Date.h"
#include "Roles.h"

const int MinYear = 1970; // WARNING: This might be too low for some time zones
const int MaxYear = 2037;
// TODO: Enforce date limits in other widgets

// TODO: Fix premature item activation

DatePickDialog::DatePickDialog(Type type, QDate date, QWidget *parent) :
    DateTimePickDialog(parent),
    type(type)
{
    setWindowTitle(tr("Select date"));

    // Make sure that the provided date is in the supported range
    date = qBound(QDate(MinYear, 1, 1), date, QDate(MaxYear, 12, 31));

    // Create the day list
    if (type == Day) {
        dList = new ExpandingListWidget(this);
        dList->setItemDelegate(new DateDayDelegate(dList));
        ui->listLayout->addWidget(dList);
        scrollers.append(Scroller(dList, 2, 1));
        connect(dList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(centerView()));
    }
    // Create the week list
    if (type == Week) {
        wList = new ExpandingListWidget(this);
        ui->listLayout->addWidget(wList);
        scrollers.append(Scroller(wList, 2, 1));
        connect(wList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(centerView()));
    }
    // Create the month list
    if (type == Month || type == Day) {
        mList = new ExpandingListWidget(this);
        for (int m = 1; m <= 12; m++)
            mList->addItem(QLocale().standaloneMonthName(m));
        ui->listLayout->addWidget(mList);
        scrollers.append(Scroller(mList, 2, 1));
        connect(mList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(centerView()));
    }
    // Create the year list
    yList = new ExpandingListWidget(this);
    for (int y = MinYear; y <= MaxYear; y++)
        yList->addItem(QString::number(y));
    ui->listLayout->addWidget(yList);
    scrollers.append(Scroller(yList, 4, MinYear));
    connect(yList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(centerView()));

    // Set up the year list
    if (type != Week) {
        yList->setCurrentRow(date.year()-MinYear);
    }
    // Set up the month list
    if (type == Month || type == Day) {
        mList->setCurrentRow(date.month()-1);
    }
    // Set up the week list
    if (type == Week) {
        int year;
        int week = Date::relWeekNumber(date, &year);

        yList->setCurrentRow(year-MinYear);

        adjustWeeks();
        wList->setCurrentRow(week-1);

        connect(yList, SIGNAL(itemSelectionChanged()), this, SLOT(adjustWeeks()));
    }
    // Set up the day list
    if (type == Day) {
        adjustDays();
        dList->setCurrentRow(date.day()-1);
        connect(mList, SIGNAL(itemSelectionChanged()), this, SLOT(adjustDays()));
        connect(yList,  SIGNAL(itemSelectionChanged()), this, SLOT(adjustDays()));
    }
}

void DatePickDialog::centerView()
{
    // Scroll day list
    if (type == Day)
        centerView(dList);

    // Scroll week list
    if (type == Week)
        centerView(wList);

    // Scroll month list
    if (type == Month || type == Day)
        centerView(mList);

    // Scroll year list
    centerView(yList);
}

void DatePickDialog::adjustDays()
{
    const int targetDays = QDate(year(), month(), 1).daysInMonth();
    const int initialDays = dList->count();

    if (initialDays < targetDays) {
        for (int i = initialDays; i < targetDays; i++)
            dList->addItem(QString::number(i+1));
    } else if (initialDays > targetDays) {
        bool reselect = row(dList) >= targetDays;

        for (int i = initialDays; i > targetDays; i--)
            delete dList->item(i-1);

        if (reselect)
            dList->setCurrentRow(targetDays-1);
    }

    for (int i = 0; i < dList->count(); i++)
        dList->item(i)->setData(DateRole, QDate(year(), month(), i+1));
}

void DatePickDialog::adjustWeeks()
{
    const int targetWeeks = QDate(year(), 12, 28).weekNumber(); // This day is always in the last week of the year
    const int initialWeeks = wList->count();

    if (initialWeeks < targetWeeks) {
        for (int i = initialWeeks; i < targetWeeks; i++)
            wList->addItem(QString::number(i+1));
    } else if (initialWeeks > targetWeeks) {
        bool reselect = row(wList) >= targetWeeks;

        for (int i = initialWeeks; i > targetWeeks; i--)
            delete wList->item(i-1);

        if (reselect)
            wList->setCurrentRow(targetWeeks-1);
    }

    // Find the first day of the first week
    QDate weekProbe(year(), 1, 1);
    const int weekday = Date::relDayOfWeek(weekProbe.dayOfWeek());
    weekProbe = weekProbe.addDays(weekday > 4 ? (1 + 7-weekday)
                                              : (1 - weekday));

    for (int i = 0; i < targetWeeks; i++) {
        wList->item(i)->setText(QString(tr("Week %1 (%2 - %3)"))
                                .arg(i+1)
                                .arg(Date::toString(weekProbe, Date::Partial))
                                .arg(Date::toString(weekProbe.addDays(6), Date::Partial)));
        weekProbe = weekProbe.addDays(7);
    }
}

int DatePickDialog::day()
{
    return row(dList) + 1;
}

int DatePickDialog::month()
{
    return row(mList) + 1;
}

int DatePickDialog::year()
{
    return row(yList) + MinYear;
}

QDate DatePickDialog::date()
{
    switch (type) {
        case Day:
            return QDate(year(), month(), day());
        case Week:
            return QDate(year(), 1, 4).addDays(row(wList)*7);
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
