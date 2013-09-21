#include "ByWeekdayCoreWidget.h"

#include "ByWeekdayColumnWidget.h"

ByWeekdayCoreWidget::ByWeekdayCoreWidget(bool reverse, QWidget *parent) :
    QWidget(parent)
{
    columnLayout = new QHBoxLayout(this);
    columnLayout->setContentsMargins(0, 0, 0, 0);
    columnLayout->setSpacing(0);

    const char *weekdays[] = { "MO", "TU", "WE", "TH", "FR", "SA", "SU" };
    for (int i = 0; i < 7; i++)
        columnLayout->addWidget(new ByWeekdayColumnWidget(weekdays[i], reverse, this), 1, Qt::AlignTop);
}

QString ByWeekdayCoreWidget::enabledRulePart() const
{
    QString result;

    // Collect rules from each enabled column
    for (int i = 0; i < 7; i++) {
        ByWeekdayColumnWidget *columnWidget = qobject_cast<ByWeekdayColumnWidget*>(columnLayout->itemAt(i)->widget());
        if (columnWidget->isEnabled()) {
            QString rule = columnWidget->rulePart();
            if (!rule.isEmpty()) {
                if (!result.isEmpty())
                    result += ',';
                result += rule;
            }
        }
    }

    return result;
}

QString ByWeekdayCoreWidget::disabledRulePart() const
{
    QString result;

    // Create a rule for each disabled column
    for (int i = 0; i < 7; i++) {
        ByWeekdayColumnWidget *columnWidget = qobject_cast<ByWeekdayColumnWidget*>(columnLayout->itemAt(i)->widget());
        if (!columnWidget->isEnabledTo(this->parentWidget())) {
            if (!result.isEmpty()) result += ',';
            result += columnWidget->id();
        }
    }

    return result;
}

int ByWeekdayCoreWidget::findColumn(QString id)
{
    for (int i = 0; i < columnLayout->count(); i++)
        if (qobject_cast<ByWeekdayColumnWidget*>(columnLayout->itemAt(i)->widget())->id() == id)
            return i;

    return -1;
}

void ByWeekdayCoreWidget::parseDay(QString day)
{
    ByWeekdayColumnWidget *columnWidget = qobject_cast<ByWeekdayColumnWidget*>(columnLayout->itemAt(findColumn(day.right(2)))->widget());

    columnWidget->addDay(day.left(day.length()-2).toInt());
}

// Enable or disable the specified column
void ByWeekdayCoreWidget::setEnabled(int day, bool enable)
{
    qobject_cast<ByWeekdayColumnWidget*>(columnLayout->itemAt(day)->widget())->setEnabled(enable);
}
