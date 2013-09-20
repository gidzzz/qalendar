#include "ByMonthWidget.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>

#include <QDate>
#include <QStringList>

#include "RecurrenceSectionLabel.h"

ByMonthWidget::ByMonthWidget(QWidget *parent) :
    QFrame(parent)
{
    this->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    /*mainLayout->setSpacing(0);*/

    QLabel *titleLabel = new RecurrenceSectionLabel(tr("By month"), this);

    monthsLayout = new QGridLayout();
    monthsLayout->setParent(mainLayout);
    monthsLayout->setContentsMargins(0, 0, 0, 0);
    monthsLayout->setSpacing(0);

    for (int m = 0; m < 12; m++) {
        QPushButton *monthButton = new QPushButton(QLocale().standaloneMonthName(m+1, QLocale::ShortFormat), this);
        monthButton->setCheckable(true);
        monthsLayout->addWidget(monthButton, m/6, m%6);
    }

    mainLayout->addWidget(titleLabel);
    mainLayout->addItem(monthsLayout);
}

QString ByMonthWidget::rulePart() const
{
    QString rule;

    for (int m = 0; m < 12; m++) {
        if (qobject_cast<QPushButton*>(monthsLayout->itemAt(m)->widget())->isChecked()) {
            if (!rule.isEmpty()) rule += ',';
            rule += QString::number(m+1);
        }
    }

    return rule;
}

void ByMonthWidget::parseRulePart(QString rule)
{
    foreach (QString part, rule.split(','))
        qobject_cast<QPushButton*>(monthsLayout->itemAt(part.toInt()-1)->widget())->setChecked(true);
}

void ByMonthWidget::clear()
{
    for (int m = 0; m < 12; m++)
        qobject_cast<QPushButton*>(monthsLayout->itemAt(m)->widget())->setChecked(false);
}
