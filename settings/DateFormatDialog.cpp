#include "DateFormatDialog.h"

#include <QPushButton>

#include "Date.h"

DateFormatDialog::DateFormatDialog(QWidget *parent) :
    RotatingDialog(parent),
    ui(new Ui::DateFormatDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    QPushButton *saveButton = new QPushButton(tr("Save"));
    ui->buttonBox->addButton(saveButton, QDialogButtonBox::AcceptRole);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

DateFormatDialog::~DateFormatDialog()
{
    delete ui;
}

void DateFormatDialog::accept()
{
    Date::setFormatStrings(ui->fullFormatWidget->format(), ui->partialFormatWidget->format());

    QDialog::accept();
}
