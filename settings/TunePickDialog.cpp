#include "TunePickDialog.h"

#include <QPushButton>

#include "TunePickSelector.h"
#include "Roles.h"

TunePickDialog::TunePickDialog(QWidget *parent, QString currentPath) :
    RotatingDialog(parent),
    ui(new Ui::TunePickDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    QPushButton *doneButton = new QPushButton(tr("Done"));
    ui->buttonBox->addButton(doneButton, QDialogButtonBox::ActionRole);
    connect(doneButton, SIGNAL(clicked()), this, SLOT(accept()));

    // Add some default sounds to choose from
    addTune("/usr/share/sounds/ui-calendar_alarm_default.aac");
    addTune("/usr/share/sounds/ui-calendar_alarm_2.aac");
    addTune("/usr/share/sounds/ui-calendar_alarm_3.aac");
    addTune("/usr/share/sounds/Beep.aac");

    // Highlight the selected sound
    for (int i = 0; i < ui->tuneList->count(); i++)
        if (ui->tuneList->item(i)->data(PathRole).toString() == currentPath)
            ui->tuneList->item(i)->setSelected(true);

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

TunePickDialog::~TunePickDialog()
{
    delete ui;
}

void TunePickDialog::addTune(QString path)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(TunePickSelector::nameForPath(path));
    item->setData(PathRole, path);
    ui->tuneList->addItem(item);
}

void TunePickDialog::accept()
{
    QList<QListWidgetItem*> selection = ui->tuneList->selectedItems();

    if (!selection.isEmpty())
        emit selected(selection.first()->data(PathRole).toString());

    QDialog::accept();
}
