#ifndef COLORCHECKDELEGATE_H
#define COLORCHECKDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

#include <QKeyEvent>

#include "CWrapper.h"
#include "Roles.h"
#include "Theme.h"
#include "Metrics.h"

class ColorCheckDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ColorCheckDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        using namespace Metrics::Item;
        using namespace Metrics::Pixmap;

        QStyledItemDelegate::paint(painter, option, QModelIndex());

        const QRect r = option.rect.adjusted(Margin, Margin, 0, 0);
        const int checkOffset = (IconSize - DetailSize) / 2;

        // Draw the background
        painter->drawPixmap(r.left(), r.top(),
                            QIcon::fromTheme(CWrapper::colorIcon(index.data(ColorRole).toInt())).pixmap(IconSize, IconSize));

        // Draw the checkbox
        painter->drawPixmap(r.left() + checkOffset,
                            r.top()  + checkOffset,
                            QIcon::fromTheme(index.data(Qt::CheckStateRole).toInt() == Qt::Checked
                                           ? "general_tickmark_checked"
                                           : "general_tickmark_unchecked").pixmap(DetailSize, 48));
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
    {
        using namespace Metrics::Item;

        return QSize(Width, Height);
    }

    bool editorEvent(QEvent *e, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        using namespace Metrics::Item;
        using namespace Metrics::Pixmap;

        if (e->type() == QEvent::MouseButtonRelease) {
            QRect checkRect = option.rect.adjusted(Margin, Margin, 0, 0);
            checkRect.setWidth(IconSize);
            checkRect.setHeight(IconSize);

            if (!checkRect.contains(static_cast<QMouseEvent*>(e)->pos()))
                return false;
        } else if (e->type() == QEvent::KeyPress) {
            if (static_cast<QKeyEvent*>(e)->key() != Qt::Key_Space
            &&  static_cast<QKeyEvent*>(e)->key() != Qt::Key_Select)
                return false;
        } else {
            return false;
        }

        return model->setData(index,
                              index.data(Qt::CheckStateRole).toInt() == Qt::Checked ? Qt::Unchecked : Qt::Checked,
                              Qt::CheckStateRole);
    }
};

#endif // COLORCHECKDELEGATE_H
