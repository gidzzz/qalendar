#include "RecurrencePickDialog.h"

#include <QDebug>

#include <QPushButton>
#include <QStringList>

#include "DatePickSelector.h"
#include "CWrapper.h"

enum Frequency
{
    Daily = 0,
    Weekly,
    Monthly,
    Yearly
};

enum Limit
{
    Forever = 0,
    Until,
    Count
};

RecurrencePickDialog::RecurrencePickDialog(QWidget *parent, CRecurrence *recurrence) :
    RotatingDialog(parent),
    ui(new Ui::RecurrencePickDialog),
    recurrence(recurrence)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(new QPushButton(tr("Done")), QDialogButtonBox::AcceptRole);

    this->setAttribute(Qt::WA_DeleteOnClose);

    DatePickSelector *dpsUntil = new DatePickSelector();
    ui->untilBox->setPickSelector(dpsUntil);

    connect(ui->enableBox, SIGNAL(toggled(bool)), ui->configWidget, SLOT(setVisible(bool)));
    connect(ui->enableBox, SIGNAL(toggled(bool)), this, SLOT(autoResize()));
    connect(ui->frequencyBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFrequencyChanged(int)));
    connect(ui->limitBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLimitTypeChanged(int)));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    onFrequencyChanged(0);

    if (recurrence->getRtype() == E_DISABLED){
        ui->configWidget->hide();
    } else {
        ui->enableBox->setChecked(true);
    }

    QStringList problems;

    // TODO: External recurrence? When is it used?
    if (recurrence->getRtype() == E_NONE)
        problems.append("This is an external recurrence, I have no idea what happens if you mess with it.");

    vector<CRecurrenceRule*> rules = recurrence->getRecurrenceRule();
    if (!rules.empty()) {
        CRecurrenceRule *rule = rules.front();
        QString ruleString = rule->getRrule().c_str();
        qDebug() << rule->toString().c_str();

        // TODO: Add a list of rules to edit
        if (rules.size() > 1)
            problems.append("Only the first rule can be edited here.");

        // TODO: Support for exceptions
        if (!recurrence->getErule().empty())
            problems.append("Exception rules cannot be edited here.");

        // TODO: Support for BYSETPOS
        if (ruleString.contains("BYSETPOS"))
            problems.append("BYSETPOS part will be discarder from the rule.");

        // TODO: Optional HMS editor?
        if (ruleString.contains("BYSECOND")
        ||  ruleString.contains("BYMINUTE")
        ||  ruleString.contains("BYHOUR"))
        {
            problems.append("Hours, minutes and seconds will be discarded from the rule.");
        }

        parseRule(ruleString);
    }

    if (!problems.isEmpty()) {
        ui->problemsInfo->show();
        ui->problemsInfo->setText(problems.join(" "));
    }

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

RecurrencePickDialog::~RecurrencePickDialog()
{
    delete ui;
}

void RecurrencePickDialog::autoResize()
{
    // TODO: Remove flickering here and in subwidgets and make the dialog shrink properly
    this->setMinimumHeight(ui->mainArea->widget()->minimumSizeHint().height());
    this->adjustSize();
}

void RecurrencePickDialog::parseRule(QString rule)
{
    QStringList ruleParts = rule.split(';');

    foreach (QString part, ruleParts) {
        if (part.startsWith("FREQ=")) {
            ui->frequencyBox->setCurrentIndex(freqFromStr(part.mid(part.indexOf('=')+1)));
        } else

        if (part.startsWith("INTERVAL=")) {
            ui->intervalBox->setValue(part.mid(part.indexOf('=')+1).toInt());
        } else

        if (part.startsWith("UNTIL=")) {
            QDateTime until = QDateTime::fromString(part.mid(part.indexOf('=')+1, 15), "yyyyMMddThhmmssZ");

            if (until.isValid()) {
                until.setTimeSpec(Qt::UTC);
            } else {
                // NOTE: Looks like the default Maemo calendar does not comply
                // with iCal spec by assigning UNTIL a local date-time. However,
                // it does not need any special treatment, as the only thing
                // that currently matters is the date.
                until = QDateTime::fromString(part.mid(part.indexOf('=')+1, 8), "yyyyMMdd");
            }

            DatePickSelector *dpsUntil = qobject_cast<DatePickSelector*>(ui->untilBox->pickSelector());
            dpsUntil->setCurrentDate(until.date());
            ui->limitBox->setCurrentIndex(Until);
        } else

        if (part.startsWith("COUNT=")) {
            ui->countBox->setValue(part.mid(part.indexOf('=')+1).toInt());
            ui->limitBox->setCurrentIndex(Count);
        } else

        if (part.startsWith("BYDAY=")) {
            ui->byWeekdayWidget->parseRulePart(part.mid(part.indexOf('=')+1));
        } else

        if (part.startsWith("BYMONTHDAY=")) {
            ui->byMonthdayWidget->parseRulePart(part.mid(part.indexOf('=')+1));
        } else

        if (part.startsWith("BYYEARDAY=")) {
            ui->byYeardayWidget->parseRulePart(part.mid(part.indexOf('=')+1));
        } else

        if (ui->frequencyBox->currentIndex() == Yearly
        &&  part.startsWith("BYWEEKNO="))
        {
            ui->byYearweekWidget->parseRulePart(part.mid(part.indexOf('=')+1));
        } else

        if (part.startsWith("BYMONTH=")) {
            ui->byMonthWidget->parseRulePart(part.mid(part.indexOf('=')+1));
        } else

            qDebug() << "WARNING: could not parse rule part" << part;
    }
}

void RecurrencePickDialog::onFrequencyChanged(int index)
{
    switch (index) {
        case Daily:
        case Weekly:
        case Monthly:
            ui->byYearweekWidget->hide();
            break;
        case Yearly:
            ui->byYearweekWidget->show();
            break;
    }
}

// Set the visibility of the controls to match the selected limit type
void RecurrencePickDialog::onLimitTypeChanged(int index)
{
    switch (index) {
        case Forever:
            ui->untilBox->hide();
            ui->countBox->hide();
            break;
        case Until:
            ui->countBox->hide();
            ui->untilBox->show();
            break;
        case Count:
            ui->untilBox->hide();
            ui->countBox->show();
            break;
    }
}

void RecurrencePickDialog::accept()
{
    if (ui->enableBox->isChecked()) {
        QStringList ruleParts;

        // Frequency
        QString freqString = freqToStr(ui->frequencyBox->currentIndex());
        if (!freqString.isEmpty())
            ruleParts.append(QString("FREQ=") + freqToStr(ui->frequencyBox->currentIndex()));

        // Limit
        QString limitString;
        switch (ui->limitBox->currentIndex()) {
            case 1: { // Until
                DatePickSelector *dpsUntil = qobject_cast<DatePickSelector*>(ui->untilBox->pickSelector());
                // NOTE: As long as the editor does not support HMS, date-only
                // format could be sufficient, but calendar-backend may parse
                // that incorrectly.
                ruleParts.append(QString("UNTIL=") + QDateTime(dpsUntil->currentDate(), QTime(23,59)).toUTC().toString("yyyyMMddThhmmssZ"));
                break;
            }
            case 2: { //Count
                ruleParts.append(QString("COUNT=") + QString::number(ui->countBox->value()));
                break;
            }
        }

        // Interval
        if (ui->intervalBox->value() > 1)
            ruleParts.append(QString("INTERVAL=") + QString::number(ui->intervalBox->value()));

        // Day of week
        QString byWeekdayString = ui->byWeekdayWidget->rulePart();
        if (!byWeekdayString.isEmpty())
            ruleParts.append(QString("BYDAY=") + byWeekdayString);

        // Day of month
        QString byMonthdayString = ui->byMonthdayWidget->rulePart();
        if (!byMonthdayString.isEmpty())
            ruleParts.append(QString("BYMONTHDAY=") + byMonthdayString);

        // Day of year
        QString byYeardayString = ui->byYeardayWidget->rulePart();
        if (!byYeardayString.isEmpty())
            ruleParts.append(QString("BYYEARDAY=") + byYeardayString);

        // Week of year
        QString byYearweekString = ui->byYearweekWidget->rulePart();
        if (!byYearweekString.isEmpty())
            ruleParts.append(QString("BYWEEKNO=") + byYearweekString);

        // Month
        QString byMonthString = ui->byMonthWidget->rulePart();
        if (!byMonthString.isEmpty())
            ruleParts.append(QString("BYMONTH=") + byMonthString);

        // Combine all of the above to form the final rule
        QString ruleString = ruleParts.join(";");
        qDebug() << ruleString;

        // Make sure that there is a rule to modify
        if (recurrence->getRecurrenceRule().empty()) {
            vector<CRecurrenceRule*> rules;
            rules.push_back(new CRecurrenceRule());
            recurrence->setRecurrenceRule(rules);
        }

        CRecurrenceRule *rule = recurrence->getRecurrenceRule().front();
        rule->rruleParser(ruleString.toAscii().data());
        rule->setRrule(ruleString.toAscii().data());

        qDebug() << "aftercheck:" << rule->getRrule().c_str() << rule->getFrequency();

        recurrence->setRtype(recurrenceType(recurrence));
    } else {
        // NOTE: The main reason why the rule is cleared here is that this is
        // the only way to quickly start afresh. If there was a dedicated clear
        // button, it might be more convenient for the user to have this rule
        // preserved.
        vector<CRecurrenceRule*> rules = recurrence->getRecurrenceRule();
        if (!rules.empty()) {
            rules.front()->rruleParser("");
            rules.front()->setRrule("");
        }

        // Mark the rule for deletion
        recurrence->setRtype(E_DISABLED);
    }

    QDialog::accept();
}

// Convert frequency option index to the corresponding ical string
QString RecurrencePickDialog::freqToStr(int frequency)
{
    switch (frequency) {
        case Daily:
            return "DAILY";
        case Weekly:
            return "WEEKLY";
        case Monthly:
            return "MONTHLY";
        case Yearly:
            return "YEARLY";
        default:
            return QString();
    }
}

int RecurrencePickDialog::freqFromStr(QString frequency)
{
    if (frequency == "DAILY")
        return Daily;
    else if (frequency == "WEEKLY")
        return Weekly;
    else if (frequency == "MONTHLY")
        return Monthly;
    else if (frequency == "YEARLY")
        return Yearly;
    else
        return Daily;
}

int RecurrencePickDialog::recurrenceType(CRecurrence *recurrence)
{
    const vector<string> rules = recurrence->getRrule();

    if (rules.empty()) {
        // Not even a single rule
        return E_NONE;
    } else if (rules.size() > 1 || !recurrence->getErule().empty()) {
        // More than one rule or some exceptions exist
        return E_COMPLEX;
    } else {
        const QString rule = rules.front().c_str();

        // Consider complex anything that does not end on a specific date
        if (!rule.contains("UNTIL"))
            return E_COMPLEX;

        // Forbidden words
        if (rule.contains("SECOND")  // SECONDLY, BYSECOND
        ||  rule.contains("MINUTE")  // MINUTELY, BYMINUTE
        ||  rule.contains("HOUR")    // HOURLY,   BYHOUR
        ||  rule.contains("INTERVAL")
        ||  rule.contains("BYYEARDAY")
        ||  rule.contains("BYWEEKNO")
        ||  rule.contains("BYMONTH") // BYMONTH, BYMONTHDAY
        ||  rule.contains("BYSETPOS"))
            return E_COMPLEX;

        // BYDAY can be used for a workday rule, otherwise it's a complex rule
        const int byDayIndex = rule.indexOf("BYDAY");
        if (byDayIndex != -1) {
            const int byDayEndIndex = rule.indexOf(';', byDayIndex);
            QString days = rule.mid(byDayIndex+6, byDayEndIndex == -1 ? -1 : byDayEndIndex-byDayIndex);

            if (days.contains("MO")
            &&  days.contains("TU")
            &&  days.contains("WE")
            &&  days.contains("TH")
            &&  days.contains("FR")
            && !days.contains("SA")
            && !days.contains("SU"))
                return E_WEEKDAY;

            return E_COMPLEX;
        }

        // Determine the type using the value of FREQ
        if (rule.contains("DAILY"))
            return E_DAILY;
        if (rule.contains("WEEKLY"))
            return E_WEEKLY;
        if (rule.contains("MONTHLY"))
            return E_MONTHLY;
        if (rule.contains("WEEKLY"))
            return E_WEEKLY;
        if (rule.contains("YEARLY"))
            return E_YEARLY;
    }

    // This case should never be reached with a valid rule
    return E_NONE;
}
