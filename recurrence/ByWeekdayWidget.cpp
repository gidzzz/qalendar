#include "ByWeekdayWidget.h"

#include <QVBoxLayout>
#include <QPushButton>

#include <QDate>

#include "RecurrenceSectionLabel.h"

ByWeekdayWidget::ByWeekdayWidget(QWidget *parent) :
    QFrame(parent)
{
    this->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *titleLabel = new RecurrenceSectionLabel(tr("By day of week"), this);

    daysLayout = new QHBoxLayout();
    daysLayout->setParent(mainLayout);
    daysLayout->setSpacing(0);

    // Create a checkable button for each weekday
    for (int i = 0; i < 7; i++) {
        QPushButton *dayButton = new QPushButton(QLocale().standaloneDayName(i+1, QLocale::ShortFormat), this);
        dayButton->setCheckable(true);
        daysLayout->addWidget(dayButton);
        connect(dayButton, SIGNAL(toggled(bool)), this, SLOT(onDayToggled(bool)));
    }

    positiveDaysWidget = new ByWeekdayCoreWidget(false, this);
    negativeDaysWidget = new ByWeekdayCoreWidget(true, this);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(positiveDaysWidget, tr("From beginning"));
    tabWidget->addTab(negativeDaysWidget, tr("From end"));

    mainLayout->addWidget(titleLabel);
    mainLayout->addItem(daysLayout);
    mainLayout->addWidget(tabWidget);
}

QString ByWeekdayWidget::rulePart() const
{
    QString result = positiveDaysWidget->disabledRulePart();

    QString rule = positiveDaysWidget->enabledRulePart();
    if (!rule.isEmpty()) {
        if (!result.isEmpty())
            result += ',';
        result += rule;
    }

    rule = negativeDaysWidget->enabledRulePart();
    if (!rule.isEmpty()) {
        if (!result.isEmpty())
            result += ',';
        result += rule;
    }

    return result;
}

void ByWeekdayWidget::parseRulePart(QString rule)
{
    foreach (QString part, rule.split(',')) {
        // General day occurences are a 2-letter code
        if ('A' <= part.at(0) && part.at(0) <= 'Z') {
            int column = positiveDaysWidget->findColumn(part);
            positiveDaysWidget->setEnabled(column, false);
            negativeDaysWidget->setEnabled(column, false);

            qobject_cast<QPushButton*>(daysLayout->itemAt(column)->widget())->setChecked(true);
        }

        // Negative day numbers start with a minus
        else if (part.at(0) == '-') {
            negativeDaysWidget->parseDay(part);
        }

        // Otherwise it should be a positive day number
        else {
            positiveDaysWidget->parseDay(part);
        }
    }
}

void ByWeekdayWidget::onDayToggled(bool checked)
{
    for (int i = 0; i < daysLayout->count(); i++) {
        if (daysLayout->itemAt(i)->widget() == this->sender()) {
            positiveDaysWidget->setEnabled(i, !checked);
            negativeDaysWidget->setEnabled(i, !checked);
            break;
        }
    }
}
