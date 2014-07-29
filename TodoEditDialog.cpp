#include "TodoEditDialog.h"

#include <limits>

#include <QDateTime>
#include <QMaemo5TimePickSelector>
#include <QSettings>

#include <CAlarm.h>
#include "CWrapper.h"

#include "DatePickSelector.h"
#include "CalendarPickSelector.h"
#include "AlarmPickSelector.h"

#include "ChangeManager.h"

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

    DatePickSelector *dps = new DatePickSelector();
    ui->dateButton->setPickSelector(dps);

    CalendarPickSelector *cps = new CalendarPickSelector();
    ui->calendarButton->setPickSelector(cps);

    AlarmPickSelector *aps = new AlarmPickSelector(E_AM_EXACTDATETIME);
    ui->alarmButton->setPickSelector(aps);

    // Make sure that AlarmPickSelector's reference date is set
    onDateChanged();

    connect(dps, SIGNAL(selected(QString)), this, SLOT(onDateChanged()));

    if (todo) {
        ui->summaryEdit->setText(QString::fromUtf8(todo->getSummary().c_str()));
        ui->descriptionEdit->setPlainText(QString::fromUtf8(todo->getDescription().c_str()));
        ui->doneBox->setChecked(todo->getStatus());
        dps->setCurrentDate(QDateTime::fromTime_t(todo->getDue()).date());
        cps->setCalendar(todo->getCalendarId());
        aps->setAlarm(todo->getAlarm());
    } else {
        todo = new CTodo();

        // Load last used settings
        QSettings settings;
        settings.beginGroup("TodoEditDialog");
        cps->setCalendar(settings.value("Calendar", 1).toInt());

        // Prepre to calculate the due date
        const time_t dueOffset = settings.value("DueOffset", 0).toInt() * 24*60*60;
        const time_t currentStamp = QDateTime::currentDateTime().toTime_t();
        QDateTime due;

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
    DatePickSelector *dps = qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector());
    CalendarPickSelector *cps = qobject_cast<CalendarPickSelector*>(ui->calendarButton->pickSelector());
    AlarmPickSelector *aps = qobject_cast<AlarmPickSelector*>(ui->alarmButton->pickSelector());

    todo->setSummary(ui->summaryEdit->text().toUtf8().data());
    todo->setDescription(ui->descriptionEdit->toPlainText().toUtf8().data());
    todo->setStatus(ui->doneBox->isChecked());
    todo->setDue(QDateTime(dps->currentDate()).toTime_t());

    aps->configureAlarm(todo);

    ChangeManager::save(todo, cps->currentId());

    // Save last used settings
    QSettings settings;
    settings.beginGroup("TodoEditDialog");
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
