#ifndef DATE_H
#define DATE_H

#include <QDate>
#include <QDateTime>

class Date
{
public:
    enum Format
    {
        Full,
        Partial
    };

    static void init();

    static QString formatString(Format format);
    static QString sanitizeFormatString(QString string, Format format);
    static void setFormatStrings(const QString &fullFormat, const QString &partialFormat);

    static int firstDayOfWeek();
    static void setFirstDayOfWeek(int firstDayOfWeek);

    static int absDayOfWeek(int relDay);
    static int relDayOfWeek(int absDay);
    static int relWeekNumber(const QDate &date, int *year = NULL);

    static QDateTime toRemote(time_t t, const QString &zone);
    static time_t toUtc(const QDateTime &t, const QString &zone);

    static QString toString(const QDate &date, Format format);
    static QString toString(const QDateTime &date, Format format, bool time = false);

private:
    static QString m_fullFormat;
    static QString m_partialFormat;
    static int m_firstDayOfWeek;

    static QString internalFormatString(Format format);

    static void loadFormatStrings();
};

#endif // DATE_H
