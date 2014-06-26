#include "ZoneWidget.h"

#include <QMaemo5ListPickSelector>
#include <QStandardItemModel>
#include <QTime>
#include <QSet>

#include <cityinfo.h>
#include <clockd/libtime.h>

ZoneWidget::ZoneWidget(QWidget *parent) :
    QMaemo5ValueButton(tr("Time zone"), parent)
{
    // Use libcityinfo to easily obtain the list of time zones (hopefully all of
    // them, otherwise /usr/share/zoneinfo/ might be a better source). Watch out
    // for duplicates, as there can be more than one city per time zone.
    QSet<QString> zones;
    Cityinfo **cities = cityinfo_get_all();
    for (int i = 0; cities && cities[i]; i++)
        zones.insert(cityinfo_get_zone(cities[i]));
    cityinfo_free_all(cities);

    QMaemo5ListPickSelector *selector = new QMaemo5ListPickSelector();
    QStandardItemModel *model = new QStandardItemModel(0, 1, selector);

    // Build model for the pick selector
    for (QSet<QString>::const_iterator z = zones.begin(); z != zones.end(); ++z) {
        const int offset = time_get_utc_offset((*z).toAscii());
        QStandardItem *item = new QStandardItem();
        item->setText(QString("%1 (GMT %2)")
                      .arg(QString(*z).replace('_', ' '))
                      .arg((offset > 0 ? '-' : '+') + QTime().addSecs(qAbs(offset)).toString("h:mm")));
        item->setData(':' + *z, Qt::UserRole);
        model->appendRow(item);
    }
    model->sort(0);

    selector->setModel(model);
    this->setPickSelector(selector);
}

QString ZoneWidget::currentZone()
{
    QMaemo5ListPickSelector *selector = static_cast<QMaemo5ListPickSelector*>(this->pickSelector());
    return selector->model()->index(selector->currentIndex(), 0).data(Qt::UserRole).toString();
}

void ZoneWidget::setCurrentZone(QString zone)
{
    QMaemo5ListPickSelector *selector = static_cast<QMaemo5ListPickSelector*>(this->pickSelector());
    QStandardItemModel *model = static_cast<QStandardItemModel*>(selector->model());

    for (int i = 0; i < model->rowCount(); i++) {
        if (model->item(i)->data(Qt::UserRole) == zone) {
            selector->setCurrentIndex(i);
            return;
        }
    }

    selector->setCurrentIndex(-1);
}
