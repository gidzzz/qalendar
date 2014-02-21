#include "RecurrencePickDialog.h"

#include <QDebug>

#include <QPushButton>
#include <QStringList>

#include "RulePickSelector.h"
#include "DatePickSelector.h"
#include "CWrapper.h"

#include "Date.h"

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

RecurrencePickDialog::RecurrencePickDialog(QWidget *parent, CRecurrence *&recurrence) :
    RotatingDialog(parent),
    ui(new Ui::RecurrencePickDialog),
    recurrence(recurrence),
    rules(recurrence->getRrule()),
    currentRule(0)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(new QPushButton(tr("Done")), QDialogButtonBox::AcceptRole);

    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->untilBox->setPickSelector(new DatePickSelector());

    const char *weekdays[] = { "MO", "TU", "WE", "TH", "FR", "SA", "SU" };
    for (int i = 1; i <= 7; i++)
        ui->weekStartBox->addItem(QLocale().standaloneDayName(Date::absDayOfWeek(i), QLocale::LongFormat), weekdays[Date::absDayOfWeek(i)-1]);

    connect(ui->enableBox, SIGNAL(toggled(bool)), ui->configWidget, SLOT(setEnabled(bool)));
    connect(ui->enableBox, SIGNAL(toggled(bool)), ui->viewButton,   SLOT(setEnabled(bool)));
    connect(ui->frequencyBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFrequencyChanged(int)));
    connect(ui->limitBox,     SIGNAL(currentIndexChanged(int)), this, SLOT(onLimitTypeChanged(int)));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    onFrequencyChanged(0);

    QStringList problems;

    // TODO: External recurrence? When is it used?
    if (recurrence->getRtype() == E_NONE)
        problems.append("This is an external recurrence, I have no idea what happens if you mess with it.");

    if (rules.empty()) {
        // Make sure that there is at least one rule, corresponding to the state
        // of the controls.
        rules.push_back(string());
    } else {
        const string &firstRule = rules.front();

        // TODO: Support for exceptions
        if (!recurrence->getErule().empty())
            problems.append("Exception rules cannot be edited here.");

        // TODO: Support for BYSETPOS
        if (string::npos != firstRule.find("BYSETPOS"))
            problems.append("BYSETPOS part will be discarder from the rule.");

        // TODO: Optional HMS editor?
        if (string::npos != firstRule.find("BYSECOND")
        ||  string::npos != firstRule.find("BYMINUTE")
        ||  string::npos != firstRule.find("BYHOUR"))
        {
            problems.append("Hours, minutes and seconds will be discarded from the rule.");
        }

        parseRule(firstRule);
    }

    if (!problems.isEmpty()) {
        ui->problemsInfo->show();
        ui->problemsInfo->setText(problems.join(" "));
    }

    // Set up the rule selector
    RulePickSelector *rps = new RulePickSelector(rules, currentRule);
    ui->viewButton->setPickSelector(rps);
    connect(rps, SIGNAL(creatingWidget()), this, SLOT(exportRule()));
    connect(rps, SIGNAL(rulesUpdated()),   this, SLOT(importRule()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

RecurrencePickDialog::~RecurrencePickDialog()
{
    delete ui;
}

void RecurrencePickDialog::clear()
{
    // Clear only the complex widgets, as there is a reasonable chance that
    // values from the basic widgets at the top can be reused.
    ui->byWeekdayWidget->clear();
    ui->byMonthdayWidget->clear();
    ui->byYeardayWidget->clear();
    ui->byYearweekWidget->clear();
    ui->byMonthWidget->clear();
}

void RecurrencePickDialog::parseRule(const string &rule)
{
    if (rule.empty()) {
        clear();
        return;
    }

    QStringList ruleParts = QString(rule.c_str()).split(';');

    // Select the default week start (Monday) in case the relevant part is missing
    ui->weekStartBox->setCurrentIndex(ui->weekStartBox->findData("MO"));

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

        if (part.startsWith("WKST=")) {
            ui->weekStartBox->setCurrentIndex(ui->weekStartBox->findData(part.mid(part.indexOf('=')+1)));
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

void RecurrencePickDialog::exportRule()
{
    rules[currentRule] = buildRule().toAscii().data();
}

void RecurrencePickDialog::importRule()
{
    // Even if the selected rule is not empty, clearing is still required so
    // that the controls are fresh before parsing.
    clear();

    // There is a possibility that the user has deleted all rules, but the list
    // should contain at least one.
    if (rules.empty()) {
        ui->enableBox->setChecked(false);
        rules.push_back(string());
    } else {
        // Set the controls to reflect the content of the selected rule
        parseRule(rules[currentRule]);
    }
}

QString RecurrencePickDialog::buildRule()
{
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

    // Week start
    QString weekStartString = ui->weekStartBox->itemData(ui->weekStartBox->currentIndex()).toString();
    if (!weekStartString.isEmpty() && weekStartString != "MO")
        ruleParts.append(QString("WKST=") + weekStartString);

    // Combine all of the above to form the final rule
    return ruleParts.join(";");
}

void RecurrencePickDialog::accept()
{
    // Update the current rule using data from the controls before saving
    rules[currentRule] = buildRule().toAscii().data();

    // It is impossible to remove rules from CRecurrence once they have been
    // added there. Put all the data into a new object instead.
    CRecurrence *recurrence = new CRecurrence();
    recurrence->setRrule(rules);
    recurrence->setRtype(ui->enableBox->isChecked() ? recurrenceType(recurrence)
                                                    : E_DISABLED);

    // Replace the old recurrence
    delete this->recurrence;
    this->recurrence = recurrence;

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
