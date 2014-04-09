#include "AlarmPickDialog.h"

#include <QPushButton>
#include <QMaemo5TimePickSelector>

#include <CAlarm.h>

#include "DatePickSelector.h"

AlarmPickDialog::AlarmPickDialog(int type, int beforeTime, int triggerTime, QWidget *parent) :
    RotatingDialog(parent),
    ui(new Ui::AlarmPickDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(new QPushButton(tr("Done")), QDialogButtonBox::AcceptRole);

    this->setAttribute(Qt::WA_DeleteOnClose);

    DatePickSelector *dps = new DatePickSelector();
    QMaemo5TimePickSelector *tps = new QMaemo5TimePickSelector();
    ui->dateButton->setPickSelector(dps);
    ui->timeButton->setPickSelector(tps);

    QButtonGroup *typeGroup = new QButtonGroup(this);
    typeGroup->addButton(ui->beforeButton, E_AM_ETIME);
    typeGroup->addButton(ui->triggerButton, E_AM_EXACTDATETIME);
    connect(typeGroup, SIGNAL(buttonClicked(int)), this, SLOT(onTypeButtonClicked(int)));

    // Set the requested mode
    if (type == E_AM_EXACTDATETIME) {
        ui->triggerButton->click();
    } else {
        ui->beforeButton->click();
    }

    // Display time before
    ui->hoursBox->setValue(beforeTime / 3600);
    ui->minutesBox->setValue(beforeTime % 3600 / 60);
    ui->secondsBox->setValue(beforeTime % 60);

    // Display trigger stamp
    QDateTime date = QDateTime::fromTime_t(triggerTime);
    dps->setCurrentDate(date.date());
    tps->setCurrentTime(date.time());

    ui->secondsBox->setSuccessor(ui->minutesBox);
    ui->minutesBox->setSuccessor(ui->hoursBox);

    connect(ui->enableBox, SIGNAL(toggled(bool)), ui->configWidget, SLOT(setEnabled(bool)));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

AlarmPickDialog::~AlarmPickDialog()
{
    delete ui;
}

void AlarmPickDialog::accept()
{
    DatePickSelector *dps = qobject_cast<DatePickSelector*>(ui->dateButton->pickSelector());
    QMaemo5TimePickSelector *tps = qobject_cast<QMaemo5TimePickSelector*>(ui->timeButton->pickSelector());

    emit selected(ui->enableBox->isChecked(),
                  ui->triggerButton->isChecked() ? E_AM_EXACTDATETIME : E_AM_ETIME,
                  ui->hoursBox->value()*3600 + ui->minutesBox->value()*60 + ui->secondsBox->value(),
                  QDateTime(dps->currentDate(), tps->currentTime()).toTime_t());

    QDialog::accept();
}

void AlarmPickDialog::onTypeButtonClicked(int type)
{
    switch (type) {
        case E_AM_ETIME:
            ui->triggerWidget->hide();
            ui->beforeWidget->show();
            break;
        case E_AM_EXACTDATETIME:
            ui->beforeWidget->hide();
            ui->triggerWidget->show();
            break;
    }

    this->adjustSize();
}
