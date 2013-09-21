#include "RecurrencePickSelector.h"

#include <QMaemo5ValueButton>

#include "RecurrencePickDialog.h"

#include "CWrapper.h"

RecurrencePickSelector::RecurrencePickSelector(QObject *parent) :
    QMaemo5AbstractPickSelector(parent),
    recurrence(NULL)
{
}

RecurrencePickSelector::~RecurrencePickSelector()
{
    delete recurrence;
}

void RecurrencePickSelector::setRecurrence(CRecurrence *recurrence)
{
    delete this->recurrence;

    this->recurrence = new CRecurrence();
    *this->recurrence = *recurrence;

    emit selected(currentValueText());
}

QString RecurrencePickSelector::currentValueText() const
{
    if (recurrence == NULL)
        return "NULL";

    switch (recurrence->getRtype()) {
        case E_DISABLED:
            return tr("Never");
        case E_NONE:
            return tr("External");
        case E_DAILY:
            return tr("Every day");
        case E_WEEKDAY:
            return tr("Every workday");
        case E_WEEKLY:
            return tr("Every week");
        case E_MONTHLY:
            return tr("Every month");
        case E_YEARLY:
            return tr("Every year");
        case E_COMPLEX:
            return tr("Complex");
        default:
            return "UNKNOWN";
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
