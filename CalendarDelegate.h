#ifndef CALENDARDELEGATE_H
#define CALENDARDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

#include <QMaemo5Style>
#include <QFontMetrics>

#include "CWrapper.h"
#include "Roles.h"
#include "Metrics.h"

class CalendarDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    CalendarDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        using namespace Metrics::Item;
        using namespace Metrics::Pixmap;

        QStyledItemDelegate::paint(painter, option, QModelIndex());

        QString name = index.data(NameRole).toString();
        QString type = CWrapper::calendarType(index.data(TypeRole).toInt());
        QIcon icon = QIcon::fromTheme(CWrapper::colorIcon(index.data(ColorRole).toInt()));
        bool visibile = index.data(VisibilityRole).toBool();

        QFont f = painter->font();
        QRect r = option.rect.adjusted(Margin, Margin, -Margin, -Margin);

        painter->save();

        if (!visibile)
            painter->setOpacity(0.25);

        // Draw calendar icon
        painter->drawPixmap(r.left(), r.top(), IconSize, IconSize, icon.pixmap(IconSize, IconSize));

        painter->setOpacity(1.0);

        r.adjust(IconSize+Margin, TextMargin, 0, -ValueMargin);

        if (!visibile)
            painter->setPen(QMaemo5Style::standardColor("DisabledTextColor"));

        // Draw calendar name
        painter->drawText(r, Qt::AlignTop|Qt::AlignLeft,
                          QFontMetrics(f).elidedText(name, Qt::ElideRight, r.width()));

        if (visibile)
            painter->setPen(QMaemo5Style::standardColor("SecondaryTextColor"));

        // Draw calendar type
        f.setPointSize(13);
        painter->setFont(f);
        painter->drawText(r, Qt::AlignBottom|Qt::AlignLeft,
                          QFontMetrics(f).elidedText(type, Qt::ElideRight, r.width()));

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
    {
        using namespace Metrics::Item;

        return QSize(Width, Height);
    }
};

#endif // CALENDARDELEGATE_H
