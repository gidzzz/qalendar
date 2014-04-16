#include "Date.h"

#include <QSettings>
#include <QLocale>

#include "ChangeManager.h"

QString Date::m_fullFormat;
QString Date::m_partialFormat;
int Date::m_firstDayOfWeek;

void Date::init()
{
    m_firstDayOfWeek = QSettings().value("Date/FirstDayOfWeek", 1).toInt();

    loadFormatStrings();
}

QString Date::formatString(Format format)
{
    switch (format) {
        case Full:
            return QSettings().value("Date/FullFormat").toString();
        case Partial:
            return QSettings().value("Date/PartialFormat").toString();
    }
}

QString Date::sanitizeFormatString(QString string, Format format)
{
    if (string.isEmpty()) {
        switch (format) {
            case Full:
                return "dddd d MMMM yyyy";
            case Partial:
                string = QLocale().dateFormat(QLocale::ShortFormat);
                return string.indexOf('M') < string.indexOf('d') ? "MM/dd" : "dd/MM";
        }
    }

    return string;
}

void Date::setFormatStrings(const QString &fullFormat, const QString &partialFormat)
{
    if (fullFormat != formatString(Full) || partialFormat != formatString(Partial)) {
        QSettings().setValue("Date/FullFormat", fullFormat);
        QSettings().setValue("Date/PartialFormat", partialFormat);
        loadFormatStrings();
        ChangeManager::bump();
    }
}

int Date::firstDayOfWeek()
{
    return m_firstDayOfWeek;
}

void Date::setFirstDayOfWeek(int firstDayOfWeek)
{
    if (firstDayOfWeek != m_firstDayOfWeek) {
        QSettings().setValue("Date/FirstDayOfWeek", m_firstDayOfWeek = firstDayOfWeek);
        ChangeManager::bump();
    }
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
    return QLocale().toString(date, internalFormatString(format));
}

QString Date::toString(const QDateTime &date, Format format, bool time)
{
    return QLocale().toString(date, internalFormatString(format) + (time ? ", hh:mm" : QString()));
}

QString Date::internalFormatString(Format format)
{
    switch (format) {
        case Full:
            return m_fullFormat;
        case Partial:
            return m_partialFormat;
    }
}

void Date::loadFormatStrings()
{
    m_fullFormat = sanitizeFormatString(formatString(Full), Full);
    m_partialFormat = sanitizeFormatString(formatString(Partial), Partial);
}
