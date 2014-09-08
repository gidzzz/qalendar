#include "EventEditDialog.h"

#include <limits>

#include <QSettings>

#include <CAlarm.h>
#include <CRecurrence.h>
#include "CWrapper.h"

#include "DatePickSelector.h"
#include "TimePickSelector.h"
#include "ZonePickSelector.h"
#include "RecurrencePickSelector.h"
#include "CalendarPickSelector.h"
#include "AlarmPickSelector.h"

#include "ChangeManager.h"
#include "Date.h"

EventEditDialog::EventEditDialog(QWidget *parent, CEvent *event) :
    ComponentEditDialog(parent),
    ui(new Ui::EventEditDialog)
{
    ui->setupUi(this);

    if (event && !event->getId().empty()) {
        event = CWrapper::details(event);

        this->setWindowTitle(tr("Edit event"));
        this->setupDeleteButton(ui->buttonBox, SLOT(deleteEvent()));
    } else {
        this->setWindowTitle(tr("New event"));
    }

    QSettings settings;

    // Set up 'from' date pickers
    DatePickSelector *dpsFrom = new DatePickSelector();
    TimePickSelector *tpsFrom = new TimePickSelector();
    ui->fromDateButton->setPickSelector(dpsFrom);
    ui->fromTimeButton->setPickSelector(tpsFrom);

    // Set up 'to' date pickers
    DatePickSelector *dpsTo = new DatePickSelector();
    TimePickSelector *tpsTo = new TimePickSelector();
    ui->toDateButton->setPickSelector(dpsTo);
    ui->toTimeButton->setPickSelector(tpsTo);

    // Set up time zone picker
    ZonePickSelector *zps;
    if (settings.value("TimeZones", false).toBool()) {
        zps = new ZonePickSelector();
        ui->zoneButton->setPickSelector(zps);
    } else {
        zps = NULL;
        ui->zoneButton->hide();
    }

    // Set up recurrence picker
    RecurrencePickSelector *rps = new RecurrencePickSelector();
    ui->repeatButton->setPickSelector(rps);

    // Set up calendar picker
    CalendarPickSelector *cps = new CalendarPickSelector();
    ui->calendarButton->setPickSelector(cps);

    // Set up alarm picker
    AlarmPickSelector *aps = new AlarmPickSelector(E_AM_ETIME);
    ui->alarmButton->setPickSelector(aps);

    // 'From' and 'to' are by default the same, equal to current time
    QDateTime currentDateTime = QDateTime::currentDateTime();
    dpsFrom->setCurrentDate(currentDateTime.date());
    tpsFrom->setCurrentTime(currentDateTime.time());
    dpsTo->setCurrentDate(currentDateTime.date());
    tpsTo->setCurrentTime(currentDateTime.time());
    duration = 0;

    // Make sure that AlarmPickSelector's reference date is set
    updateAlarmReference();

    // Enable field constraints
    connect(ui->allDayBox, SIGNAL(toggled(bool)), this, SLOT(onAllDayChanged(bool)));
    connect(dpsFrom, SIGNAL(selected(QString)), this, SLOT(onFromChanged()));
    connect(tpsFrom, SIGNAL(selected(QString)), this, SLOT(onFromChanged()));
    connect(dpsTo, SIGNAL(selected(QString)), this, SLOT(onToChanged()));
    connect(tpsTo, SIGNAL(selected(QString)), this, SLOT(onToChanged()));

    // Global settings have been dealt with, switch to local scope
    settings.beginGroup("EventEditDialog");

    if (event) {
        // Configure time
        QString zone;
        QDateTime from;
        QDateTime to;
        if (zps) {
            // Display time in the selected time zone
            zone = event->getTzid().c_str();
            from = Date::toRemote(event->getDateStart(), zone);
            to = Date::toRemote(event->getDateEnd(), zone);
            zps->setCurrentZone(event->getTzid().c_str());
        } else {
            // Display local time
            from = QDateTime::fromTime_t(event->getDateStart());
            to = QDateTime::fromTime_t(event->getDateEnd());
        }

        ui->summaryEdit->setText(QString::fromUtf8(event->getSummary().c_str()));
        ui->locationEdit->setText(QString::fromUtf8(event->getLocation().c_str()));
        ui->descriptionEdit->setPlainText(QString::fromUtf8(event->getDescription().c_str()));
        ui->allDayBox->setChecked(event->getAllDay());
        dpsFrom->setCurrentDate(from.date());
        tpsFrom->setCurrentTime(from.time());
        dpsTo->setCurrentDate(to.date());
        tpsTo->setCurrentTime(to.time());
        cps->setCalendar(event->getCalendarId());
        aps->setAlarm(event->getAlarm(), zone);
    } else {
        event = new CEvent();

        // Load last used settings
        ui->allDayBox->setChecked(settings.value("AllDay", false).toBool());
        cps->setCalendar(settings.value("Calendar", 1).toInt());
        aps->setSecondsBefore(settings.value("Alarm", -1).toInt());

        // Some additional processing for time zones
        if (zps) {
            const QString zone = settings.value("TimeZone", QString()).toString();
            if (!zone.isEmpty()) {
                // Display time in the selected time zone
                currentDateTime = Date::toRemote(currentDateTime.toTime_t(), zone);
                dpsFrom->setCurrentDate(currentDateTime.date());
                tpsFrom->setCurrentTime(currentDateTime.time());
                dpsTo->setCurrentDate(currentDateTime.date());
                tpsTo->setCurrentTime(currentDateTime.time());

                // Load last used setting
                zps->setCurrentZone(zone);
            }
        }

        ui->summaryEdit->setFocus();
    }

    // Make sure that a valid calendar is selected (for cloned birthdays)
    if (int calendarId = event->getCalendarId()) {
        cps->setCalendar(calendarId);
    } else {
        cps->setCalendar(settings.value("Calendar", 1).toInt());
    }

    // Make sure that the recurrence exists, as required by the recurrence edit dialog
    if (event->getRecurrence()) {
        rps->setRecurrence(event->getRecurrence());
    } else {
        CRecurrence emptyRecurrence;
        emptyRecurrence.setRtype(E_DISABLED);
        rps->setRecurrence(&emptyRecurrence);
    }

    this->setupSaveButton(ui->buttonBox, SLOT(saveEvent()));

    ui->mainArea->widget()->layout()->activate();

    this->setFeatures(ui->dialogLayout, ui->buttonBox);

    this->event = event;
}

EventEditDialog::~EventEditDialog()
{
    delete event;

    delete ui;
}

// Set the value of date/time selectors
void EventEditDialog::setFromTo(QDateTime from, QDateTime to)
{
    // Convert time if time zones are enabled
    if (ZonePickSelector *zps = qobject_cast<ZonePickSelector*>(ui->zoneButton->pickSelector())) {
        const QString zone = zps->currentZone();
        if (!zone.isEmpty()) {
            from = Date::toRemote(from.toTime_t(), zone);
            to = Date::toRemote(to.toTime_t(), zone);
        }
    }

    qobject_cast<DatePickSelector*>(ui->fromDateButton->pickSelector())->setCurrentDate(from.date());
    qobject_cast<TimePickSelector*>(ui->fromTimeButton->pickSelector())->setCurrentTime(from.time());

    qobject_cast<DatePickSelector*>(ui->toDateButton->pickSelector())->setCurrentDate(to.date());
    qobject_cast<TimePickSelector*>(ui->toTimeButton->pickSelector())->setCurrentTime(to.time());
}

void EventEditDialog::setAllDay(bool allDay)
{
    ui->allDayBox->setChecked(allDay);
}

void EventEditDialog::updateAlarmReference()
{
    DatePickSelector *dpsFrom = qobject_cast<DatePickSelector*>(ui->fromDateButton->pickSelector());
    TimePickSelector *tpsFrom = qobject_cast<TimePickSelector*>(ui->fromTimeButton->pickSelector());
    AlarmPickSelector *aps = qobject_cast<AlarmPickSelector*>(ui->alarmButton->pickSelector());

    aps->setReferenceDate(QDateTime(dpsFrom->currentDate(), ui->allDayBox->isChecked() ? QTime(00,00) : tpsFrom->currentTime()));
}

void EventEditDialog::onAllDayChanged(bool enabled)
{
    ui->fromTimeButton->setHidden(enabled);
    ui->toTimeButton->setHidden(enabled);

    updateAlarmReference();
}

void EventEditDialog::onFromChanged()
{
    DatePickSelector *dpsFrom = qobject_cast<DatePickSelector*>(ui->fromDateButton->pickSelector());
    DatePickSelector *dpsTo = qobject_cast<DatePickSelector*>(ui->toDateButton->pickSelector());
    TimePickSelector *tpsFrom = qobject_cast<TimePickSelector*>(ui->fromTimeButton->pickSelector());
    TimePickSelector *tpsTo = qobject_cast<TimePickSelector*>(ui->toTimeButton->pickSelector());

    QDateTime from(dpsFrom->currentDate(), tpsFrom->currentTime());

    // Calculate the new 'to' value in an overflow-proof way
    QDateTime to = QDateTime::fromTime_t((time_t) from.toTime_t() > std::numeric_limits<time_t>::max() - duration
                                         ? std::numeric_limits<time_t>::max()
                                         : from.toTime_t() + duration);

    dpsTo->setCurrentDate(to.date());
    tpsTo->setCurrentTime(to.time());

    updateAlarmReference();
}

void EventEditDialog::onToChanged()
{
    DatePickSelector *dpsFrom = qobject_cast<DatePickSelector*>(ui->fromDateButton->pickSelector());
    DatePickSelector *dpsTo = qobject_cast<DatePickSelector*>(ui->toDateButton->pickSelector());
    TimePickSelector *tpsFrom = qobject_cast<TimePickSelector*>(ui->fromTimeButton->pickSelector());
    TimePickSelector *tpsTo = qobject_cast<TimePickSelector*>(ui->toTimeButton->pickSelector());

    QDateTime from(dpsFrom->currentDate(), tpsFrom->currentTime());
    QDateTime to(dpsTo->currentDate(), tpsTo->currentTime());

    // 'From' must be earlier than or equal to 'to'
    if (from > to) {
        from = to;
        dpsFrom->setCurrentDate(from.date());
        tpsFrom->setCurrentTime(from.time());
    }

    duration = to.toTime_t() - from.toTime_t();
}

void EventEditDialog::saveEvent()
{
    // Prepare for saving settings
    QSettings settings;
    settings.beginGroup("EventEditDialog");

    // Get pick selectors
    DatePickSelector *dpsFrom = qobject_cast<DatePickSelector*>(ui->fromDateButton->pickSelector());
    DatePickSelector *dpsTo = qobject_cast<DatePickSelector*>(ui->toDateButton->pickSelector());
    TimePickSelector *tpsFrom = qobject_cast<TimePickSelector*>(ui->fromTimeButton->pickSelector());
    TimePickSelector *tpsTo = qobject_cast<TimePickSelector*>(ui->toTimeButton->pickSelector());
    ZonePickSelector *zps = qobject_cast<ZonePickSelector*>(ui->zoneButton->pickSelector());
    RecurrencePickSelector *rps = qobject_cast<RecurrencePickSelector*>(ui->repeatButton->pickSelector());
    CalendarPickSelector *cps = qobject_cast<CalendarPickSelector*>(ui->calendarButton->pickSelector());
    AlarmPickSelector *aps = qobject_cast<AlarmPickSelector*>(ui->alarmButton->pickSelector());

    // Handle all-day events
    const bool allDay = ui->allDayBox->isChecked();
    const QDateTime from(dpsFrom->currentDate(), allDay ? QTime(00,00) : tpsFrom->currentTime());
    const QDateTime to(dpsTo->currentDate(), allDay ? QTime(23,59) : tpsTo->currentTime());

    // Set event properties
    event->setSummary(ui->summaryEdit->text().toUtf8().data());
    event->setLocation(ui->locationEdit->text().toUtf8().data());
    event->setDescription(ui->descriptionEdit->toPlainText().toUtf8().data());
    event->setAllDay(allDay);

    // Set time
    QString zone;
    if (zps) {
        zone = zps->currentZone();
        event->setDateStart(Date::toUtc(from, zone));
        event->setDateEnd(Date::toUtc(to, zone));
        event->setTzid(zone.toAscii().data());
        settings.setValue("TimeZone", zone == CMulticalendar::getSystemTimeZone().c_str() ? QString() : zone);
    } else {
        event->setDateStart(from.toTime_t());
        event->setDateEnd(from.toTime_t());
    }

    // Set alarm
    aps->configureAlarm(event, zone);
    // NOTE: It might be a good idea to notify the user if the alarm was in the
    // past and impossible to set, or add some constraints in the alarm picker.

    // Set recurrence
    CRecurrence *recurrence = rps->currentRecurrence();
    if (recurrence->getRtype() == E_DISABLED) {
        // Remove the recurrence, as it has been marked for deletion
        event->removeRecurrence();
    } else {
        event->setRecurrence(recurrence);
        // NOTE: The stock Maemo calendar saves the UNTIL part of the recurrence
        // also in the event, but the lack of it does not appear to break
        // anything, so do not bother.
    }

    ChangeManager::save(event, cps->currentId());

    // Save last used settings
    settings.setValue("AllDay", ui->allDayBox->isChecked());
    settings.setValue("Calendar", cps->currentId());
    settings.setValue("Alarm", aps->currentSecondsBefore());
    // NOTE: Sometimes it might be convenient to have the settings saved
    // regardless of the way in which the window was closed (save/cancel).
    // NOTE: Another possibly useful thing would be to have the last used
    // calendar synchronized between all component editors.

    this->accept();
}

void EventEditDialog::deleteEvent()
{
    if (ChangeManager::drop(this, event))
        this->accept();
}
