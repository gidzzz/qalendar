#include "BirthdayEditDialog.h"

#include "CWrapper.h"

#include "AlarmPickSelector.h"

#include "ChangeManager.h"

BirthdayEditDialog::BirthdayEditDialog(QWidget *parent, CBdayEvent *event) :
    ComponentEditDialog(parent),
    ui(new Ui::BirthdayEditDialog)
{
    ui->setupUi(this);

    event = CWrapper::details(event);

    // Determine the closest future occurrence
    QDateTime currentDate = QDateTime::currentDateTime();
    QDateTime closestDate = QDateTime::fromTime_t(event->getDateStart());
    closestDate = QDateTime(QDate(currentDate.date().year(),
                                  closestDate.date().month(),
                                  closestDate.date().day()),
                            closestDate.time());
    if (closestDate <= currentDate)
        closestDate = closestDate.addYears(1);

    // Set up alarm picker
    AlarmPickSelector *aps = new AlarmPickSelector(E_AM_EXACTDATETIME);
    aps->setReferenceDate(closestDate);
    aps->setAlarm(event->getAlarm(), QString());
    ui->alarmButton->setPickSelector(aps);

    this->setupSaveButton(ui->buttonBox, SLOT(saveEvent()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);

    this->event = event;
}

BirthdayEditDialog::~BirthdayEditDialog()
{
    delete event;

    delete ui;
}

void BirthdayEditDialog::saveEvent()
{
    qobject_cast<AlarmPickSelector*>(ui->alarmButton->pickSelector())->configureAlarm(event, QString());

    ChangeManager::save(event);

    this->accept();
}
