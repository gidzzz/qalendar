#include "ZonePickSelector.h"

#include "ZonePickDialog.h"

ZonePickSelector::ZonePickSelector(QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
}

QWidget* ZonePickSelector::widget(QWidget *parent)
{
    ZonePickDialog *dialog = new ZonePickDialog(parent, zone);

    connect(dialog, SIGNAL(selected(QString)), this, SLOT(setCurrentZone(QString)));

    return dialog;
}

QString ZonePickSelector::currentValueText() const
{
    return ZonePickDialog::displayName(zone);
}

QString ZonePickSelector::currentZone() const
{
    return zone;
}

void ZonePickSelector::setCurrentZone(const QString &zone)
{
    this->zone = zone;

    emit selected(currentValueText());
}
