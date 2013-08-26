#ifndef COMPONENTLISTDELEGATE_H
#define COMPONENTLISTDELEGATE_H

#include <CComponent.h>

#include <QStyledItemDelegate>
#include <QPainter>

#include <QMaemo5Style>
#include <QFontMetrics>
#include <QDateTime>

#include "CWrapper.h"
#include "Roles.h"
#include "Theme.h"
#include "Metrics.h"

#include "DateHeadingWidget.h"

class ComponentListDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ComponentListDelegate(QObject *parent) : QStyledItemDelegate(parent) { }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        using namespace Metrics::Item;
        using namespace Metrics::Pixmap;

        if (index.data(HeadingRole).toBool()) {
            painter->save();

            DateHeadingWidget::paint(*painter, option.rect, index.data(DateRole).toDate());

            painter->restore();
        } else {
            ComponentInstance *instance = qvariant_cast<ComponentInstance*>(index.data(ComponentRole));

            if (!instance) return;

            QStyledItemDelegate::paint(painter, option, QModelIndex());

            CComponent *component = instance->component;

            QFont f = painter->font();
            QRect r = option.rect.adjusted(Margin, Margin, -Margin, -Margin);

            // Get info about the component
            QString summary = CWrapper::simplify(component->getSummary(), TextMaxChars);
            QString location = CWrapper::simplify(component->getLocation(), ValueMaxChars);
            QDateTime startDate = QDateTime::fromTime_t(instance->stamp);
            QDateTime endDate   = QDateTime::fromTime_t(instance->end());
            bool alarm = component->getAlarm();

            // Get info required for the customized icon
            QDate date = index.data(DateRole).toDate();
            QIcon background = QIcon::fromTheme(CWrapper::colorIcon(index.data(ColorRole).toInt()));

            // Draw the background of the icon
            painter->drawPixmap(r.left(), r.top(), IconSize, IconSize, background.pixmap(IconSize, IconSize));

            // Draw the icon
            if (component->getType() == E_TODO) {
                // Todo
                painter->drawPixmap(r.left() + (IconSize - DetailSize) / 2,
                                    r.top()  + (IconSize - DetailSize) / 2,
                                    QIcon::fromTheme("calendar_todo").pixmap(DetailSize, DetailSize));
            } else if (component->getType() == E_BDAY) {
                // Birthday
                painter->drawPixmap(r.left() + (IconSize - DetailSize) / 2,
                                    r.top()  + (IconSize - DetailSize) / 2,
                                    QIcon::fromTheme("calendar_birthday").pixmap(DetailSize, DetailSize));
            } else {
                // Normal event
                QString timeText;

                if (component->getAllDay()) {
                    timeText = tr("All\nday");
                    if (startDate.date() < date)
                        timeText = "..." + timeText;
                    if (date < endDate.date())
                        timeText = timeText + "...";
                } else {
                    if (startDate.date() < date && date < endDate.date()) {
                        timeText = "...";
                    } else if (startDate.date() < date) {
                        timeText = "...\n" + endDate.toString("hh:mm");
                    } else if (date < endDate.date()) {
                        timeText = startDate.toString("hh:mm") + "\n...";
                    } else {
                        timeText = startDate.toString("hh:mm") + "\n" + endDate.toString("hh:mm");
                    }
                }

                painter->save();

                f.setPointSize(13);
                painter->setFont(f);
                painter->setPen(QMaemo5Style::standardColor("ReversedTextColor"));

                painter->drawText(r.left(), r.top(), IconSize, IconSize,
                                  Qt::AlignHCenter | Qt::AlignVCenter,
                                  timeText);

                painter->restore();
            }

            r.adjust(IconSize+Margin, 0, 0, 0);

            // Draw alarm indicator
            if (alarm) {
                painter->drawPixmap(r.right()  - Margin - EmblemSize,
                                    r.bottom() - Margin - EmblemSize,
                                    QIcon::fromTheme("calendar_alarm").pixmap(EmblemSize, EmblemSize));
            }

            Qt::AlignmentFlag summaryAlignment;

            // Configure the layout based on the availability of location info
            if (location.isEmpty()) {
                summaryAlignment = Qt::AlignVCenter;
            } else {
                summaryAlignment = Qt::AlignTop;
                r.adjust(0, TextMargin, 0, -ValueMargin);

                painter->save();

                f.setPointSize(13);
                painter->setFont(f);
                painter->setPen(QMaemo5Style::standardColor("SecondaryTextColor"));

                // Draw location
                painter->drawText(r, Qt::AlignLeft|Qt::AlignBottom,
                                  QFontMetrics(f).elidedText(location, Qt::ElideRight, r.width()));

                painter->restore();
            }

            // Draw summary
            painter->drawText(r, Qt::AlignLeft|summaryAlignment,
                              QFontMetrics(f).elidedText(summary, Qt::ElideRight, r.width()));
        }
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex &index) const
    {
        using namespace Metrics::Item;

        return QSize(Width, index.data(HeadingRole).toBool() ? 30 : Height);
    }
};

#endif // COMPONENTLISTDELEGATE_H
