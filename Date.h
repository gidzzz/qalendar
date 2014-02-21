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

    static int firstDayOfWeek();
    static void setFirstDayOfWeek(int firstDayOfWeek);

    static int absDayOfWeek(int relDay);
    static int relDayOfWeek(int absDay);
    static int relWeekNumber(const QDate &date, int *year = NULL);

    static QString toString(const QDate &date, Format format);
    static QString toString(const QDateTime &date, Format format, bool time = false);

private:
    static int m_firstDayOfWeek;

    static QString formatString(Format format);
};

#endif // DATE_H
