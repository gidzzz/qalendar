#include "ByWeekdayColumnWidget.h"

#include <QPushButton>

#include "DayNumberDialog.h"

const int MaxOccurences = 53;

ByWeekdayColumnWidget::ByWeekdayColumnWidget(QString weekdayId, bool reverse, QWidget *parent) :
    QWidget(parent),
    weekdayId(weekdayId),
    reverse(reverse)
{
    listLayout = new QVBoxLayout(this);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);

    QPushButton *addButton = new QPushButton(QIcon::fromTheme("general_add"), QString(), this);
    addButton->setMinimumWidth(1);
    listLayout->addWidget(addButton);

    connect(addButton, SIGNAL(clicked()), this, SLOT(showAddDialog()));
}

QString ByWeekdayColumnWidget::id() const
{
    return weekdayId;
}

QString ByWeekdayColumnWidget::rulePart() const
{
    QString result;

    for (int i = 0; i < days.size(); i++) {
        if (i != 0) result += ',';
        result += QString::number(days.at(i)) + weekdayId;
    }

    return result;
}

// Show a dialog which allows to add a day to the list
void ByWeekdayColumnWidget::showAddDialog()
{
    DayNumberDialog *dnd = new DayNumberDialog(MaxOccurences, this);

    if (dnd->exec() == QDialog::Accepted)
        addDay(reverse ? -dnd->value() : dnd->value());

    delete dnd;
}

void ByWeekdayColumnWidget::addDay(int day)
{
    int i;

    // Find a place to insert the day number
    for (i = 0; i < days.size(); i++) {
        if (day == days.at(i)) {
            // Abort to avoid duplicates
            return;
        } else if (day < days.at(i)) {
            // The proper index has been found,
            break;
        }
    }

    // Set up the required day button
    QPushButton *dayButton = new QPushButton(QString::number(reverse ? -day : day), this);
    dayButton->setMinimumWidth(1);
    listLayout->insertWidget(i, dayButton);
    connect(dayButton, SIGNAL(clicked()), this, SLOT(onDayClicked()));

    // Insert the day number into the internal list
    days.insert(i, day);
}

// React to a click on a day button, by removing it from the list
void ByWeekdayColumnWidget::onDayClicked()
{
    QPushButton *dayButton = qobject_cast<QPushButton*>(this->sender());

    // Remove the last number button
    listLayout->removeWidget(dayButton);
    dayButton->hide();
    dayButton->deleteLater();

    // Remove the day from the internal list
    days.removeOne(reverse ? -dayButton->text().toInt() : dayButton->text().toInt());
}

void ByWeekdayColumnWidget::setEnabled(bool enable)
{
    // Set visibility of number buttons
    for (int i = 0; i < listLayout->count()-1; i++)
        listLayout->itemAt(i)->widget()->setVisible(enable);

    QWidget::setEnabled(enable);
}
