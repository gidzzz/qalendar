#ifndef DATEDAYDELEGATE_H
#define DATEDAYDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QMaemo5Style>
#include <QFontMetrics>

#include <QDate>

#include "CWrapper.h"
#include "Roles.h"
#include "Metrics.h"

class DateDayDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DateDayDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        using namespace Metrics::Item;

        QStyledItemDelegate::paint(painter, option, QModelIndex());

        QDate date = index.data(DateRole).toDate();

        QFont f = painter->font();
        QRect r = option.rect.adjusted(Margin, Margin+TextMargin, -Margin, -Margin-ValueMargin);

        painter->save();

        // Draw day number
        painter->drawText(r, Qt::AlignTop|Qt::AlignHCenter,
                          QString::number(date.day()));

        // Draw weekday name
        f.setPointSize(13);
        painter->setFont(f);
        painter->setPen(QMaemo5Style::standardColor("SecondaryTextColor"));
        painter->drawText(r, Qt::AlignBottom|Qt::AlignHCenter,
                          QDate::shortDayName(date.dayOfWeek()));

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
    {
        using namespace Metrics::Item;

        return QSize(1, Height);
    }
};

#endif // DATEDAYDELEGATE_H
