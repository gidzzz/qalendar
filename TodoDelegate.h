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
        // Use the default delegate for group headings
        if (index.data(HeadingRole).toBool())
            return QStyledItemDelegate::paint(painter, option, index);

        using namespace Metrics::Item;
        using namespace Metrics::Pixmap;

        ColorCheckDelegate::paint(painter, option, index);

        CTodo *todo = qvariant_cast<CTodo*>(index.data(TodoRole));

        if (!todo) return;

        QString summary = CWrapper::simplify(todo->getSummary(), TextMaxChars);
        QDateTime due = QDateTime::QDateTime::fromTime_t(todo->getDue());
        bool alarm = todo->getAlarm();

        QFont f = painter->font();
        QRect r = option.rect.adjusted(Margin+IconSize+Margin, Margin+TextMargin, -Margin, -Margin-ValueMargin);
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
        if (alarm) {
            painter->drawPixmap(r.right()  - Margin - EmblemSize,
                                r.bottom() - Margin - EmblemSize,
                                QIcon::fromTheme("calendar_alarm").pixmap(EmblemSize, EmblemSize));
        }

        painter->restore();
    }

    bool editorEvent(QEvent *e, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        // Group headings should not be checkable
        return index.data(HeadingRole).toBool() ? false : ColorCheckDelegate::editorEvent(e, model, option, index);
    }
};

#endif // TODODELEGATE_H
