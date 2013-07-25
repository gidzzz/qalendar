#include "CalendarsConfigSelector.h"

#include <QMaemo5ValueButton>

#include <CMulticalendar.h>

#include "CalendarsConfigDialog.h"

CalendarsConfigSelector::CalendarsConfigSelector(QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
}

QString CalendarsConfigSelector::currentValueText() const
{
    int error;
    return tr("%n visible", "", CMulticalendar::MCInstance()->getVisibleCalendarCount(error));
}

QWidget* CalendarsConfigSelector::widget(QWidget *parent)
{
    CalendarsConfigDialog *dialog = new CalendarsConfigDialog(parent);

    connect(dialog, SIGNAL(destroyed()), this, SLOT(reload()));

    return dialog;
}

void CalendarsConfigSelector::reload()
{
    emit selected(currentValueText());
}
