#ifndef DETAILEDCALENDARDELEGATE_H
#define DETAILEDCALENDARDELEGATE_H

#include "ColorCheckDelegate.h"

#include <QMaemo5Style>
#include <QFontMetrics>

#include "CWrapper.h"

#include "Metrics.h"

class DetailedCalendarDelegate : public ColorCheckDelegate
{
    Q_OBJECT

public:
    DetailedCalendarDelegate(QObject *parent) : ColorCheckDelegate(parent) { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        using namespace Metrics::Item;
        using namespace Metrics::Pixmap;

        ColorCheckDelegate::paint(painter, option, index);

        QString name = index.data(NameRole).toString();

        QString details = QString(CWrapper::calendarType(index.data(TypeRole).toInt()))
                          + ", " + tr("%n events", "", index.data(EventCountRole).toInt())
                          + ", " + tr("%n tasks", "", index.data(TodoCountRole).toInt())
                          + ", " + tr("%n notes", "", index.data(JournalCountRole).toInt());

        QFont f = painter->font();
        QRect r = option.rect.adjusted(Margin+IconSize+Margin, Margin+TextMargin, -Margin, -Margin-ValueMargin);

        painter->save();

        // Draw calendar name
        painter->drawText(r.left(), r.top(), r.width(), r.height(),
                          Qt::AlignTop|Qt::AlignLeft,
                          QFontMetrics(f).elidedText(name, Qt::ElideRight, r.width()));

        // Draw calendar details
        f.setPointSize(13);
        painter->setFont(f);
        painter->setPen(QMaemo5Style::standardColor("SecondaryTextColor"));
        painter->drawText(r.left(), r.top(), r.width(), r.height(),
                          Qt::AlignBottom|Qt::AlignLeft,
                          QFontMetrics(f).elidedText(details, Qt::ElideRight, r.width()));

        painter->restore();
    }
};

#endif // DETAILEDCALENDARDELEGATE_H
