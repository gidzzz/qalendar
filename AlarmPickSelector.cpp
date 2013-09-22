#include "AlarmPickSelector.h"

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
    if (seconds < 0) return tr("None");

    const int h = seconds / 3600;
    const int m = seconds % 3600 / 60;
    const int s = seconds % 60;

    QStringList parts;

    const int numFieldsSet = (bool) h + (bool) m + (bool) s;

    if (numFieldsSet == 0) {
        return tr("Exact");
    } else if (numFieldsSet == 1) {
        return h ? tr("%n hour(s) before", "", h) :
               m ? tr("%n minute(s) before", "", m) :
                   tr("%n second(s) before", "", s);
    } else {
        return QString(tr("%1:%2:%3 before"))
               .arg(QString::number(h), 2, '0')
               .arg(QString::number(m), 2, '0')
               .arg(QString::number(s), 2, '0');
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
