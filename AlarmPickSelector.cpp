#include "AlarmPickSelector.h"

#include <QMaemo5ValueButton>

#include "AlarmPickDialog.h"

AlarmPickSelector::AlarmPickSelector(QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
    secondsBefore = -1;
}

void AlarmPickSelector::setSecondsBefore(int seconds)
{
    secondsBefore = seconds;

    emit selected(currentValueText());
}

QString AlarmPickSelector::currentValueText() const
{
    return textForSeconds(secondsBefore);
}

QString AlarmPickSelector::textForSeconds(int seconds)
{
    if (seconds < 0) return "None";

    const int h = seconds / 3600;
    const int m = seconds % 3600 / 60;
    const int s = seconds % 60;

    QStringList parts;

    const int numFieldsSet = (bool) h + (bool) m + (bool) s;

    if (numFieldsSet == 0) {
        return "Exact";
    } else if (numFieldsSet == 1) {
        return h ? QString::number(h) + " hour(s) before" :
               m ? QString::number(m) + " minute(s) before" :
                   QString::number(s) + " second(s) before";
    } else {
        return QString::number(h).rightJustified(2, '0') + ":" +
               QString::number(m).rightJustified(2, '0') + ":" +
               QString::number(s).rightJustified(2, '0') + " before";
    }
}

int AlarmPickSelector::currentSecondsBefore() const
{
    return secondsBefore;
}

QWidget* AlarmPickSelector::widget(QWidget *parent)
{
    AlarmPickDialog *dialog = new AlarmPickDialog(secondsBefore, parent);

    connect(dialog, SIGNAL(selected(int)), this, SLOT(onSelected(int)));

    return dialog;
}

void AlarmPickSelector::onSelected(int seconds)
{
    setSecondsBefore(seconds);
}
