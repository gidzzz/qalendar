#include "ByDayWidget.h"

#include <QVBoxLayout>

#include "RecurrenceSectionLabel.h"

ByDayWidget::ByDayWidget(QWidget *parent) :
    QFrame(parent)
{
    this->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    titleLabel = new RecurrenceSectionLabel(this);
    titleLabel->setIndent(5);

    positiveDaysWidget = new ByDayCoreWidget(false, this);
    negativeDaysWidget = new ByDayCoreWidget(true, this);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(positiveDaysWidget, tr("From beginning"));
    tabWidget->addTab(negativeDaysWidget, tr("From end"));

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(tabWidget);
}

void ByDayWidget::setMaximum(int maximum)
{
    // HACK, change it to something nicer
    if (maximum == 31)
        titleLabel->setText(tr("By day of month"));
    if (maximum == 53)
        titleLabel->setText(tr("By week of year"));
    if (maximum == 366)
        titleLabel->setText(tr("By day of year"));

    positiveDaysWidget->setMaximum(maximum);
    negativeDaysWidget->setMaximum(maximum);
}

QString ByDayWidget::rulePart() const
{
    QString result;

    // Positive days
    QList<int>
    days = positiveDaysWidget->selectedDays();
    for (int i = 0; i < days.size(); i++) {
        if (i != 0) result += ',';
        result += QString::number(days.at(i));
    }

    // Negative days
    days = negativeDaysWidget->selectedDays();
    if (!result.isEmpty() && !days.isEmpty()) result += ',';
    for (int i = 0; i < days.size(); i++) {
        if (i != 0) result += ',';
        result += QString::number(days.at(i));
    }

    return result;
}

void ByDayWidget::parseRulePart(QString rule)
{
    QList<int> positiveDays;
    QList<int> negativeDays;

    foreach (QString part, rule.split(',')) {
        // Negative day numbers start with a minus
        if (part.at(0) == '-') {
            negativeDays.append(part.toInt());
        }
        // Otherwise it should be a positive day number
        else {
            positiveDays.append(part.toInt());
        }
    }

    positiveDaysWidget->selectDays(positiveDays);
    negativeDaysWidget->selectDays(negativeDays);
}
