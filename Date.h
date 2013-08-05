#ifndef DATE_H
#define DATE_H

#include <QDate>
#include <QDateTime>

namespace Date
{
    enum Format
    {
        Full,
        Partial
    };

    inline QString formatString(Format format)
    {
        switch (format) {
            case Full:
                return "dddd d MMMM yyyy";
            case Partial:
                return "dd/MM";
        }
    }

    inline QString toString(const QDate &date, Format format)
    {
        return QLocale().toString(date, formatString(format));
    }

    inline QString toString(const QDateTime &date, Format format, bool time = false)
    {
        return QLocale().toString(date, formatString(format) + (time ? ", hh:mm" : QString()));
    }
}

#endif // DATE_H
