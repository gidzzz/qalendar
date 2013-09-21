#include "TodoEditDialog.h"

#include <limits>

#include <QDateTime>
#include <QMaemo5TimePickSelector>
#include <QSettings>

#include <CAlarm.h>
#include "CWrapper.h"

#include "DatePickSelector.h"
#include "CalendarPickSelector.h"

#include "ChangeManager.h"

TodoEditDialog::TodoEditDialog(QWidget *parent, CTodo *todo) :
    ComponentEditDialog(parent),
    ui(new Ui::TodoEditDialog)
{
    ui->setupUi(this);

    if (todo) todo = CWrapper::details(todo);

    DatePickSelector *dps = new DatePickSelector();
    ui->dateButton->setPickSelector(dps);

    CalendarPickSelector *cps = new CalendarPickSelector();
    ui->calendarButton->setPickSelector(cps);

    DatePickSelector *adps = new DatePickSelector();
    ui->alarmDateButton->setPickSelector(adps);

    QMaemo5TimePickSelector *atps = new QMaemo5TimePickSelector();
    ui->alarmTimeButton->setPickSelector(atps);

    connect(ui->alarmBox, SIGNAL(toggled(bool)), ui->alarmDateButton, SLOT(setVisible(bool)));
    connect(ui->alarmBox, SIGNAL(toggled(bool)), ui->alarmTimeButton, SLOT(setVisible(bool)));
    connect(dps, SIGNAL(selected(QString)), this, SLOT(onDateChanged()));
    connect(adps, SIGNAL(selected(QString)), this, SLOT(onAlarmChanged()));
    connect(atps, SIGNAL(selected(QString)), this, SLOT(onAlarmChanged()));

    if (todo) {
        this->setWindowTitle(tr("Edit task"));

        ui->summaryEdit->setText(QString::fromUtf8(todo->getSummary().c_str()));
        ui->descriptionEdit->setPlainText(QString::fromUtf8(todo->getDescription().c_str()));
        ui->doneBox->setChecked(todo->getStatus());
        dps->setCurrentDate(QDateTime::fromTime_t(todo->getDue()).date());
        cps->setCalendar(todo->getCalendarId());

        if (todo->getAlarm()) {
            QDateTime trigger = QDateTime::fromTime_t(todo->getAlarm()->getTrigger());
            adps->setCurrentDate(trigger.date());
            atps->setCurrentTime(trigger.time());
            ui->alarmBox->setChecked(true);
        }

        this->setupDeleteButton(ui->buttonBox, SLOT(deleteTodo()));
    } else {
        this->setWindowTitle(tr("New task"));

        todo = new CTodo();

        // Load last used settings
        QSettings settings;
        settings.beginGroup("TodoEditDialog");
        cps->setCalendar(settings.value("Calendar", 1).toInt());

        ui->doneBox->hide();

        ui->summaryEdit->setFocus();
    }

    // The default alarm is one day before the deadline, at noon
    if (!todo->getAlarm()) {
        adps->setCurrentDate(dps->currentDate().addDays(-1));
        atps->setCurrentTime(QTime(12,00));
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
    qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector())->setCurrentDate(due);
}

void TodoEditDialog::onDateChanged()
{
    DatePickSelector *dps = qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector());
    DatePickSelector *adps = qobject_cast<DatePickSelector*>(ui->alarmDateButton->pickSelector());
    QMaemo5TimePickSelector *atps = qobject_cast<QMaemo5TimePickSelector*>(ui->alarmTimeButton->pickSelector());

    QDateTime due(dps->currentDate(), QTime(00,00));
    QDateTime alarm(adps->currentDate(), atps->currentTime());

    // Calculate the new alarm trigger value in an under/overflow-proof way
    if (alarmOffset > 0) {
        alarm = QDateTime::fromTime_t((time_t) due.toTime_t() > std::numeric_limits<time_t>::max() - alarmOffset
                                    ? std::numeric_limits<time_t>::max()
                                    : due.toTime_t() + alarmOffset);
    } else {
        alarm = QDateTime::fromTime_t((time_t) due.toTime_t() < alarmOffset
                                    ? 0
                                    : due.toTime_t() + alarmOffset);
    }
    // NOTE: toTime_t() is a deceptive name, the return type is uint.

    adps->setCurrentDate(alarm.date());
    atps->setCurrentTime(alarm.time());
}

void TodoEditDialog::onAlarmChanged()
{
    DatePickSelector *dps = qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector());
    DatePickSelector *adps = qobject_cast<DatePickSelector*>(ui->alarmDateButton->pickSelector());
    QMaemo5TimePickSelector *atps = qobject_cast<QMaemo5TimePickSelector*>(ui->alarmTimeButton->pickSelector());

    alarmOffset = QDateTime(adps->currentDate(), atps->currentTime()).toTime_t()
                - QDateTime(dps->currentDate(), QTime(00,00)).toTime_t();
}

void TodoEditDialog::saveTodo()
{
    DatePickSelector *dps = qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector());
    CalendarPickSelector *cps = qobject_cast<CalendarPickSelector*>(ui->calendarButton->pickSelector());
    DatePickSelector *adps = qobject_cast<DatePickSelector*>(ui->alarmDateButton->pickSelector());
    QMaemo5TimePickSelector *atps = qobject_cast<QMaemo5TimePickSelector*>(ui->alarmTimeButton->pickSelector());

    todo->setSummary(ui->summaryEdit->text().toUtf8().data());
    todo->setDescription(ui->descriptionEdit->toPlainText().toUtf8().data());
    todo->setStatus(ui->doneBox->isChecked());
    todo->setDue(QDateTime(dps->currentDate()).toTime_t());

    if (ui->alarmBox->isChecked()) {
        QDateTime trigger(adps->currentDate(), atps->currentTime());
        // WARNING: Setting the alarm to a moment in the past can lead to a segfault
        if (trigger > QDateTime::currentDateTime()) {
            todo->setAlarm(trigger.toTime_t(), E_AM_EXACTDATETIME);
        } else {
            todo->removeAlarm();
        }
    } else {
        todo->removeAlarm();
    }

    ChangeManager::save(todo, cps->currentId());

    // Save last used settings
    QSettings settings;
    settings.beginGroup("TodoEditDialog");
    settings.setValue("Calendar", cps->currentId());
    // TODO: Save alarm settings? What date/time to use?

    this->accept();
}

void TodoEditDialog::deleteTodo()
{
    if (ChangeManager::drop(this, todo))
        this->accept();
}
