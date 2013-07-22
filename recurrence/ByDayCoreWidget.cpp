#include "ByDayCoreWidget.h"

#include <QPushButton>

#include "DayNumberDialog.h"

const int GridWidth = 7;

ByDayCoreWidget::ByDayCoreWidget(bool reverse, QWidget *parent) :
    QWidget(parent),
    maximum(1),
    reverse(reverse)
{
    listLayout = new QGridLayout(this);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);

    // Set column count and size constraints
    for (int i = 0; i < GridWidth; i++) {
        listLayout->setColumnStretch(i, 1);
        listLayout->setColumnMinimumWidth(i, 1);
    }

    QPushButton *addButton = new QPushButton(QIcon::fromTheme("general_add"), QString(), this);
    connect(addButton, SIGNAL(clicked()), this, SLOT(showAddDialog()));

    QPoint pos = mapToGrid(listLayout->count());
    listLayout->addWidget(addButton, pos.y(), pos.x(), Qt::AlignTop);
}

// Set the maximum day number (minimum is always 1)
void ByDayCoreWidget::setMaximum(int maximum)
{
    this->maximum = maximum;
}

QList<int> ByDayCoreWidget::selectedDays()
{
    return days;
}

void ByDayCoreWidget::selectDays(QList<int> days)
{
    // NOTE: This function is possibly quite slow if the list is long.
    foreach (int day, days)
        addDay(day);
}

// Map slot number to a point in the grid
QPoint ByDayCoreWidget::mapToGrid(int index) const
{
    return QPoint(index % GridWidth, index / GridWidth);
}

// Show a dialog which allows to add a day to the list
void ByDayCoreWidget::showAddDialog()
{
    DayNumberDialog *dnd = new DayNumberDialog(maximum, this);

    if (dnd->exec() == QDialog::Accepted)
        addDay(reverse ? -dnd->value() : dnd->value());

    dnd->deleteLater();
}

// React to a click on a day button, by removing it from the list
void ByDayCoreWidget::onDayClicked()
{
    QPushButton *dayButton = qobject_cast<QPushButton*>(this->sender());

    // Temporarily remove the add button
    QWidget *addButton = listLayout->takeAt(listLayout->count()-1)->widget();

    // Remove the number button at the end if the lits, not the one clicked,
    // to avoid empty spaces in the grid layout
    QWidget *buttonToRemove = listLayout->takeAt(listLayout->count()-1)->widget();
    buttonToRemove->hide();
    buttonToRemove->deleteLater();

    // Remove the day from the internal list and update labels on the buttons
    // to make it look like the clicked button was removed
    days.removeOne(reverse ? -dayButton->text().toInt()
                           :  dayButton->text().toInt());
    updateButtons();

    // Reinsert the add button
    QPoint pos = mapToGrid(listLayout->count());
    listLayout->addWidget(addButton, pos.y(), pos.x(), Qt::AlignTop);
}

void ByDayCoreWidget::addDay(int day)
{
    int i;

    // Find a place to insert the day number
    for (i = 0; i < days.size(); i++) {
        if (day == days.at(i)) {
            // Abort to avoid duplicates
            return;
        } else if (day < days.at(i)) {
            // The proper index has been found
            break;
        }
    }

    // Temporarily remove the add button
    QWidget *addButton = listLayout->takeAt(listLayout->count()-1)->widget();

    // Create a day button
    QPushButton *dayButton = new QPushButton(this);
    connect(dayButton, SIGNAL(clicked()), this, SLOT(onDayClicked()));
    dayButton->setMinimumWidth(1);

    // Add the day button to the layout, at the end of the list
    QPoint pos = mapToGrid(listLayout->count());
    listLayout->addWidget(dayButton, pos.y(), pos.x(), Qt::AlignTop);

    // Put the add button back
    pos = mapToGrid(listLayout->count());
    listLayout->addWidget(addButton, pos.y(), pos.x(), Qt::AlignTop);

    // Insert the day number into the internal list and update labels on all
    // buttons, as many of them might be wrong due to sorting
    days.insert(i, day);
    updateButtons();
}

// Sync button labels with day numbers in the internal list
void ByDayCoreWidget::updateButtons()
{
    for (int i = 0; i < days.size(); i++)
        qobject_cast<QPushButton*>(listLayout->itemAt(i)->widget())->setText(QString::number(reverse ? -days.at(i)
                                                                                                     :  days.at(i)));
}
