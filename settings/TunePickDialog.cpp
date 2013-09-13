#include "TunePickDialog.h"

#include <QFileDialog>
#include <QPushButton>

#include "TunePickSelector.h"
#include "Roles.h"

TunePickDialog::TunePickDialog(QWidget *parent, QString currentPath) :
    RotatingDialog(parent),
    ui(new Ui::TunePickDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    QPushButton *moreButton = new QPushButton(tr("More"));
    QPushButton *doneButton = new QPushButton(tr("Done"));
    ui->buttonBox->addButton(moreButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(doneButton, QDialogButtonBox::ActionRole);
    connect(moreButton, SIGNAL(clicked()), this, SLOT(browse()));
    connect(doneButton, SIGNAL(clicked()), this, SLOT(accept()));

    // Add some default sounds to choose from
    addTune("/usr/share/sounds/ui-calendar_alarm_default.aac");
    addTune("/usr/share/sounds/ui-calendar_alarm_2.aac");
    addTune("/usr/share/sounds/ui-calendar_alarm_3.aac");
    addTune("/usr/share/sounds/Beep.aac");

    // Try to locate the selected sound in the default list
    int i = 0;
    while (i < ui->tuneList->count()
       &&  ui->tuneList->item(i)->data(PathRole).toString() != currentPath)
           i++;

    // If the sound is not in the default list, one more item will be added
    if (i == ui->tuneList->count()) {
        addTune(currentPath, true);
        customItem = ui->tuneList->item(0);
        customItem->setSelected(true);
    } else {
        customItem = NULL;
        ui->tuneList->item(i)->setSelected(true);
    }

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

TunePickDialog::~TunePickDialog()
{
    delete ui;
}

void TunePickDialog::addTune(QString path, bool front)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(TunePickSelector::nameForPath(path));
    item->setData(PathRole, path);
    ui->tuneList->insertItem(front ? 0 : ui->tuneList->count(), item);
}

void TunePickDialog::browse()
{
    this->setEnabled(false);

    QString path = QFileDialog::getOpenFileName(this, tr("Add alarm tone"),
                                                "/home/user/MyDocs/.sounds/",
                                                "*.aac *.mp3 *.wav");

    if (!path.isEmpty()) {
        if (customItem) {
            // Reuse the existing custom sound item
            customItem->setText(TunePickSelector::nameForPath(path));
            customItem->setData(PathRole, path);
        } else {
            // Add a new item to display the custom sound
            addTune(path, true);
            customItem = ui->tuneList->item(0);

            // Update the geometry to take the new item into account
            ui->tuneList->updateGeometry();
        }

        customItem->setSelected(true);
    }

    this->setEnabled(true);
}

void TunePickDialog::accept()
{
    QList<QListWidgetItem*> selection = ui->tuneList->selectedItems();

    if (!selection.isEmpty())
        emit selected(selection.first()->data(PathRole).toString());

    QDialog::accept();
}
