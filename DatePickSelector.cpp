#include "DatePickSelector.h"

#include <QMaemo5ValueButton>

#include "DatePickDialog.h"

#include "Date.h"

DatePickSelector::DatePickSelector(QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
    date = QDate::currentDate();
}

QDate DatePickSelector::currentDate() const
{
    return date;
}

void DatePickSelector::setCurrentDate(const QDate &date)
{
    if (this->date != date) {
        this->date = date;
        emit selected(currentValueText());
    }
}

QString DatePickSelector::currentValueText() const
{
    return Date::toString(date, Date::Full);
}

QWidget* DatePickSelector::widget(QWidget *parent)
{
    DatePickDialog *dialog = new DatePickDialog(DatePickDialog::Day, date, parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog, SIGNAL(selected(QDate)), this, SLOT(onSelected(QDate)));

    return dialog;
}

void DatePickSelector::onSelected(QDate date)
{
    setCurrentDate(date);
}
