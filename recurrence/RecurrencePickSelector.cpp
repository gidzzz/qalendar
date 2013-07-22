#include "RecurrencePickSelector.h"

#include <QMaemo5ValueButton>

#include "RecurrencePickDialog.h"

#include "CWrapper.h"

RecurrencePickSelector::RecurrencePickSelector(QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
    recurrence = NULL;
}

void RecurrencePickSelector::setRecurrence(CRecurrence *recurrence)
{
    this->recurrence = recurrence;

    emit selected(currentValueText());
}

QString RecurrencePickSelector::currentValueText() const
{
    if (recurrence == NULL)
        return "NULL";

    switch (recurrence->getRtype()) {
        case E_DISABLED:
            return "Never";
        case E_NONE:
            return "External";
        case E_DAILY:
            return "Every day";
        case E_WEEKDAY:
            return "Every workday";
        case E_WEEKLY:
            return "Every week";
        case E_MONTHLY:
            return "Every month";
        case E_YEARLY:
            return "Every year";
        case E_COMPLEX:
            return "Complex";
        default:
            return "(unknown)";
    }
}

CRecurrence* RecurrencePickSelector::currentRecurrence() const
{
    return recurrence;
}

QWidget* RecurrencePickSelector::widget(QWidget *parent)
{
    RecurrencePickDialog *dialog = new RecurrencePickDialog(parent, recurrence);

    connect(dialog, SIGNAL(accepted()), this, SLOT(onSelected()));

    return dialog;
}

void RecurrencePickSelector::onSelected()
{
    emit selected(currentValueText());
}
