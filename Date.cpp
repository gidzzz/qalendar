#include "Date.h"

#include <QSettings>
#include <QLocale>

int Date::m_firstDayOfWeek;

void Date::init()
{
    m_firstDayOfWeek = QSettings().value("Date/FirstDayOfWeek", 1).toInt();
}

int Date::firstDayOfWeek()
{
    return m_firstDayOfWeek;
}

void Date::setFirstDayOfWeek(int firstDayOfWeek)
{
    m_firstDayOfWeek = firstDayOfWeek;

    QSettings().setValue("Date/FirstDayOfWeek", firstDayOfWeek);
}

// Map relative (user) to absolute (Qt) day of week
int Date::absDayOfWeek(int relDayOfWeek)
{
    return 1 + ((relDayOfWeek-1) + (m_firstDayOfWeek-1)    ) % 7;
}

// Map absolute (Qt) to relative (user) day of week
int Date::relDayOfWeek(int absDayOfWeek)
{
    return 1 + ((absDayOfWeek-1) - (m_firstDayOfWeek-1) + 7) % 7;
}

// Calculate relative (user) week number and its year
int Date::relWeekNumber(const QDate &date, int *year)
{
    const QDate givenMid = date.addDays(1 - relDayOfWeek(date.dayOfWeek()) + 3);
    const QDate firstDay = QDate(givenMid.year(), 1, 1);
    const QDate firstMid = firstDay.addDays(((4-1) - (relDayOfWeek(firstDay.dayOfWeek())-1) + 7) % 7);

    if (year) *year = givenMid.year();

    return firstMid.daysTo(givenMid) / 7 + 1;
}

QString Date::toString(const QDate &date, Format format)
{
    return QLocale().toString(date, formatString(format));
}

QString Date::toString(const QDateTime &date, Format format, bool time)
{
    return QLocale().toString(date, formatString(format) + (time ? ", hh:mm" : QString()));
}

QString Date::formatString(Format format)
{
    switch (format) {
        case Full:
            return "dddd d MMMM yyyy";
        case Partial: {
            const QString localeShortFormat = QLocale().dateFormat(QLocale::ShortFormat);
            return localeShortFormat.indexOf('M') < localeShortFormat.indexOf('d') ? "MM/dd" : "dd/MM";
        }
    }
}
