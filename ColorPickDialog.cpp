#include "ColorPickDialog.h"

#include <QPushButton>

#include "CWrapper.h"

ColorPickDialog::ColorPickDialog(int color, QWidget *parent) :
    RotatingDialog(parent),
    ui(new Ui::ColorPickDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(new QPushButton(tr("Done")), QDialogButtonBox::AcceptRole);

    this->setAttribute(Qt::WA_DeleteOnClose);

    colorGroup = new QButtonGroup(this);

    // Add a button for each color
    for (int i = 0; i < COLOUR_NEXT_FREE; i++) {
        // Create a button
        QPushButton *colorButton = new QPushButton(this);
        colorButton->setIcon(QIcon::fromTheme(CWrapper::colorIcon(i)));
        colorButton->setCheckable(true);

        //  Check the button if it matches the current color
        if (i == color)
            colorButton->setChecked(i == color);

        // Add the button to collections
        ui->colorLayout->addWidget(colorButton, i/5, i%5);
        colorGroup->addButton(colorButton, i);
    }

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

ColorPickDialog::~ColorPickDialog()
{
    delete ui;
}

void ColorPickDialog::accept()
{
    emit selected(colorGroup->checkedId());

    QDialog::accept();
}
