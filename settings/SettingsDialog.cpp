#include "SettingsDialog.h"

#include <QStandardItemModel>

#include <QSettings>

#include "CalendarsConfigSelector.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    RotatingDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(new QPushButton(tr("Save")), QDialogButtonBox::AcceptRole);

    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->calendarsButton->setPickSelector(new CalendarsConfigSelector());

    QSettings settings;
    ui->deleteEventsButton->setPickSelector(buildDeleteSelector(settings.value("DeleteEventsAfter", 0).toInt()));
    ui->deleteTodosButton->setPickSelector(buildDeleteSelector(settings.value("DeleteTodosAfter", 0).toInt()));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;

}

QMaemo5ListPickSelector* SettingsDialog::buildDeleteSelector(int selectValue)
{
    QMaemo5ListPickSelector *selector;
    QStandardItemModel *model;
    QStandardItem *item;

    selector = new QMaemo5ListPickSelector();
    model = new QStandardItemModel(0, 1, selector);
    item = new QStandardItem(tr("Never")); item->setData(0, Qt::UserRole);
    model->appendRow(item);
    item = new QStandardItem(tr("Older than %n week(s)", "", 1)); item->setData(60*60*24*7, Qt::UserRole);
    model->appendRow(item);
    item = new QStandardItem(tr("Older than %n week(s)", "", 2)); item->setData(60*60*24*7*2, Qt::UserRole);
    model->appendRow(item);
    item = new QStandardItem(tr("Older than %n month(s)", "", 1)); item->setData(60*60*24*31, Qt::UserRole);
    model->appendRow(item);
    item = new QStandardItem(tr("Older than %n year(s)", "", 1)); item->setData(60*60*24*366, Qt::UserRole);
    model->appendRow(item);
    selector->setModel(model);

    for (int i = 0; i < model->rowCount(); i++) {
        if (selectValue == model->item(i)->data(Qt::UserRole).toInt()) {
            selector->setCurrentIndex(i);
            break;
        }
    }

    return selector;
}

void SettingsDialog::saveSettings()
{
    QMaemo5ListPickSelector *des = static_cast<QMaemo5ListPickSelector*>(ui->deleteEventsButton->pickSelector());
    QMaemo5ListPickSelector *dts = static_cast<QMaemo5ListPickSelector*>(ui->deleteTodosButton->pickSelector());

    QSettings settings;
    settings.setValue("DeleteEventsAfter", des->model()->index(des->currentIndex(), 0).data(Qt::UserRole).toInt());
    settings.setValue("DeleteTodosAfter", des->model()->index(dts->currentIndex(), 0).data(Qt::UserRole).toInt());

    this->accept();
}
