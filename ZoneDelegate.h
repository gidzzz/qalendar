#ifndef ZONEDELEGATE_H
#define ZONEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

#include <QMaemo5Style>
#include <QFontMetrics>

#include "Metrics.h"

class ZoneDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ZoneDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        using namespace Metrics::Item;

        QStyledItemDelegate::paint(painter, option, QModelIndex());

        QFont f = painter->font();
        QRect r = option.rect.adjusted(Margin, Margin, -Margin, -Margin);

        // Unpack zone info
        const QStringList info = index.data(Qt::DisplayRole).toString().split(QChar(31));
        const QString name = info.at(0);
        const QString region = info.at(1);
        const QString offset = info.at(2);

        // Draw offset
        painter->drawText(r, Qt::AlignRight|Qt::AlignVCenter, offset);
        r.adjust(0, 0, -(Margin + QFontMetrics(f).width(offset)), 0);

        Qt::AlignmentFlag nameAlignment;

        // Configure the layout based on the availability of region info
        if (region.isEmpty()) {
            nameAlignment = Qt::AlignVCenter;
        } else {
            nameAlignment = Qt::AlignTop;
            r.adjust(0, TextMargin, 0, -ValueMargin);

            painter->save();

            f.setPointSize(13);
            painter->setFont(f);
            painter->setPen(QMaemo5Style::standardColor("SecondaryTextColor"));

            // Draw region
            painter->drawText(r, Qt::AlignLeft|Qt::AlignBottom, region);

            painter->restore();
        }

        // Draw name
        painter->drawText(r, Qt::AlignLeft|nameAlignment, name);
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
    {
        using namespace Metrics::Item;

        return QSize(Width, Height);
    }
};

#endif // ZONEDELEGATE_H
