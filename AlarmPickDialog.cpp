#include "AlarmPickDialog.h"

#include <QPushButton>

#include "CWrapper.h"

AlarmPickDialog::AlarmPickDialog(int seconds, QWidget *parent) :
    RotatingDialog(parent),
    ui(new Ui::AlarmPickDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(new QPushButton(tr("Done")), QDialogButtonBox::AcceptRole);

    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->hoursBox->setValue(seconds / 3600);
    ui->minutesBox->setValue(seconds % 3600 / 60);
    ui->secondsBox->setValue(seconds % 60);

    ui->minutesBox->setSuccessor(ui->hoursBox);
    ui->secondsBox->setSuccessor(ui->minutesBox);

    connect(ui->enableBox, SIGNAL(toggled(bool)), ui->hoursBox, SLOT(setEnabled(bool)));
    connect(ui->enableBox, SIGNAL(toggled(bool)), ui->minutesBox, SLOT(setEnabled(bool)));
    connect(ui->enableBox, SIGNAL(toggled(bool)), ui->secondsBox, SLOT(setEnabled(bool)));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

AlarmPickDialog::~AlarmPickDialog()
{
    delete ui;
}

void AlarmPickDialog::accept()
{
    emit selected(ui->enableBox->isChecked() ? ui->hoursBox->value() * 3600 +
                                               ui->minutesBox->value() * 60 +
                                               ui->secondsBox->value()
                                             : -1);

    QDialog::accept();
}
