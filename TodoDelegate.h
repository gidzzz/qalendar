#ifndef TODODELEGATE_H
#define TODODELEGATE_H

#include "ColorCheckDelegate.h"

#include <QMaemo5Style>
#include <QFontMetrics>
#include <QDateTime>

#include <CTodo.h>

#include "CWrapper.h"

#include "Date.h"
#include "Theme.h"
#include "Metrics.h"

class TodoDelegate : public ColorCheckDelegate
{
    Q_OBJECT

public:
    TodoDelegate(QObject *parent) : ColorCheckDelegate(parent) { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        using namespace Metrics::Item;
        using namespace Metrics::Pixmap;

        ColorCheckDelegate::paint(painter, option, index);

        QFont f = painter->font();
        QRect r = option.rect.adjusted(Margin+IconSize+Margin, Margin+TextMargin, -Margin, -Margin-ValueMargin);

        if (index.data(HeadingRole).toBool()) {
            QString name = index.data(NameRole).toString();
            QString stats = index.data(DetailsRole).toString();

            // Draw todo counts
            painter->drawText(r, Qt::AlignVCenter|Qt::AlignRight, stats);

            r.adjust(0, 0, -QFontMetrics(f).width(stats), 0);

            // Draw calendar name
            painter->drawText(r, Qt::AlignVCenter|Qt::AlignLeft,
                              QFontMetrics(f).elidedText(name, Qt::ElideRight, r.width()));

            return;
        } else {
            CTodo *todo = qvariant_cast<CTodo*>(index.data(TodoRole));

            if (!todo) return;

            QString summary = CWrapper::simplify(todo->getSummary(), TextMaxChars);
            QDateTime due = QDateTime::QDateTime::fromTime_t(todo->getDue());
            bool alarm = todo->getAlarm();

            QColor secondaryColor = QMaemo5Style::standardColor("SecondaryTextColor");

            painter->save();

            if (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) {
                // Paint completed tasks differently
                f.setStrikeOut(todo->getStatus());
                painter->setFont(f);
                painter->setPen(secondaryColor);
            } else if (due.date() < index.data(DateRole).toDate()) {
                // Highlight overdue tasks
                painter->setPen(QMaemo5Style::standardColor("ActiveTextColor"));
            }

            // Draw summary
            painter->drawText(r, Qt::AlignTop|Qt::AlignLeft,
                              QFontMetrics(f).elidedText(summary, Qt::ElideRight, r.width()));

            // Draw due date
            f.setStrikeOut(false);
            f.setPointSize(13);
            painter->setFont(f);
            painter->setPen(secondaryColor);
            painter->drawText(r, Qt::AlignBottom|Qt::AlignLeft,
                              QFontMetrics(f).elidedText(Date::toString(due, Date::Full), Qt::ElideRight, r.width()));

            // Draw alarm indicator
            if (alarm)
                painter->drawPixmap(r.right()  - Margin - EmblemSize,
                                    r.bottom() - Margin - EmblemSize,
                                    QIcon::fromTheme("calendar_alarm").pixmap(EmblemSize, EmblemSize));

            painter->restore();
        }
    }
};

#endif // TODODELEGATE_H
