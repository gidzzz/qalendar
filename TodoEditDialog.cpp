#include "TodoEditDialog.h"

#include <limits>

#include <QSettings>

#include <CAlarm.h>
#include "CWrapper.h"

#include "DatePickSelector.h"
#include "ZonePickSelector.h"
#include "CalendarPickSelector.h"
#include "AlarmPickSelector.h"

#include "ChangeManager.h"
#include "Date.h"

TodoEditDialog::TodoEditDialog(QWidget *parent, CTodo *todo) :
    ComponentEditDialog(parent),
    ui(new Ui::TodoEditDialog),
    defaultDue(true)
{
    ui->setupUi(this);

    if (todo && !todo->getId().empty()) {
        todo = CWrapper::details(todo);

        this->setWindowTitle(tr("Edit task"));
        this->setupDeleteButton(ui->buttonBox, SLOT(deleteTodo()));
    } else {
        this->setWindowTitle(tr("New task"));
    }

    QSettings settings;

    // Set up date picker
    DatePickSelector *dps = new DatePickSelector();
    ui->dateButton->setPickSelector(dps);

    // Set up time zone picker
    ZonePickSelector *zps;
    if (settings.value("TimeZones", false).toBool()) {
        zps = new ZonePickSelector();
        ui->zoneButton->setPickSelector(zps);
    } else {
        zps = NULL;
        ui->zoneButton->hide();
    }

    // Set up calendar picker
    CalendarPickSelector *cps = new CalendarPickSelector();
    ui->calendarButton->setPickSelector(cps);

    // Set up alarm picker
    AlarmPickSelector *aps = new AlarmPickSelector(E_AM_EXACTDATETIME);
    ui->alarmButton->setPickSelector(aps);

    // Make sure that AlarmPickSelector's reference date is set
    onDateChanged();

    connect(dps, SIGNAL(selected(QString)), this, SLOT(onDateChanged()));

    if (todo) {
        // Configure time
        QString zone;
        QDateTime due;
        if (zps) {
            // Display time in the selected time zone
            zone = todo->getTzid().c_str();
            due = Date::toRemote(todo->getDue(), zone);
            zps->setCurrentZone(todo->getTzid().c_str());
        } else {
            // Display local time
            due = QDateTime::fromTime_t(todo->getDue());
        }

        ui->summaryEdit->setText(QString::fromUtf8(todo->getSummary().c_str()));
        ui->descriptionEdit->setPlainText(QString::fromUtf8(todo->getDescription().c_str()));
        ui->doneBox->setChecked(todo->getStatus());
        dps->setCurrentDate(due.date());
        cps->setCalendar(todo->getCalendarId());
        aps->setAlarm(todo->getAlarm(), zone);
    } else {
        todo = new CTodo();

        // Load last used settings
        settings.beginGroup("TodoEditDialog");
        cps->setCalendar(settings.value("Calendar", 1).toInt());

        // Prepre to calculate the due date
        const time_t dueOffset = settings.value("DueOffset", 0).toInt() * 24*60*60;
        time_t currentStamp = QDateTime::currentDateTime().toTime_t();
        QDateTime due;

        // Some additional processing for time zones
        if (zps) {
            const QString zone = settings.value("TimeZone", QString()).toString();
            if (!zone.isEmpty()) {
                // Display time in the selected time zone
                currentStamp = Date::toRemote(currentStamp, zone).toTime_t();

                // Load last used setting
                zps->setCurrentZone(zone);
            }
        }

        // Calculate the due date in an under/overflow-proof way
        if (dueOffset > 0) {
            due = QDateTime::fromTime_t(currentStamp > std::numeric_limits<time_t>::max() - dueOffset
                                      ? std::numeric_limits<time_t>::max()
                                      : currentStamp + dueOffset);
        } else {
            due = QDateTime::fromTime_t(currentStamp < -dueOffset
                                      ? 0
                                      : currentStamp + dueOffset);
        }
        dps->setCurrentDate(due.date());

        ui->doneBox->hide();

        ui->summaryEdit->setFocus();
    }

    this->setupSaveButton(ui->buttonBox, SLOT(saveTodo()));

    ui->editArea->widget()->layout()->activate();

    this->setFeatures(ui->dialogLayout, ui->buttonBox);

    this->todo = todo;
}

TodoEditDialog::~TodoEditDialog()
{
    delete todo;

    delete ui;
}

void TodoEditDialog::setDue(QDate due)
{
    // Convert time if time zones are enabled
    if (ZonePickSelector *zps = qobject_cast<ZonePickSelector*>(ui->zoneButton->pickSelector())) {
        const QString zone = zps->currentZone();
        if (!zone.isEmpty())
            due = Date::toRemote(QDateTime(due, QTime(00,00)).toTime_t(), zone).date();
    }

    qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector())->setCurrentDate(due);

    defaultDue = false;
}

void TodoEditDialog::onDateChanged()
{
    DatePickSelector *dps = qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector());
    AlarmPickSelector *aps = qobject_cast<AlarmPickSelector*>(ui->alarmButton->pickSelector());

    aps->setReferenceDate(QDateTime(dps->currentDate(), QTime(00,00)));
}

void TodoEditDialog::saveTodo()
{
    // Prepare for saving settings
    QSettings settings;
    settings.beginGroup("TodoEditDialog");

    // Get pick selectors
    DatePickSelector *dps = qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector());
    ZonePickSelector *zps = qobject_cast<ZonePickSelector*>(ui->zoneButton->pickSelector());
    CalendarPickSelector *cps = qobject_cast<CalendarPickSelector*>(ui->calendarButton->pickSelector());
    AlarmPickSelector *aps = qobject_cast<AlarmPickSelector*>(ui->alarmButton->pickSelector());

    // Set todo properties
    todo->setSummary(ui->summaryEdit->text().toUtf8().data());
    todo->setDescription(ui->descriptionEdit->toPlainText().toUtf8().data());
    todo->setStatus(ui->doneBox->isChecked());

    // Set time
    const QDateTime due = QDateTime(dps->currentDate(), QTime(00,00));
    QString zone;
    if (zps) {
        zone = zps->currentZone();
        todo->setDue(Date::toUtc(due, zone));
        todo->setTzid(zone.toAscii().data());
        settings.setValue("TimeZone", zone == CMulticalendar::getSystemTimeZone().c_str() ? QString() : zone);
    } else {
        todo->setDue(due.toTime_t());
    }

    // Set alarm
    aps->configureAlarm(todo, zone);

    ChangeManager::save(todo, cps->currentId());

    // Save last used settings
    settings.setValue("Calendar", cps->currentId());
    if (defaultDue)
        settings.setValue("DueOffset", QDate::currentDate().daysTo(dps->currentDate()));
    // TODO: Save alarm settings? What date/time to use?

    this->accept();
}

void TodoEditDialog::deleteTodo()
{
    if (ChangeManager::drop(this, todo))
        this->accept();
}
