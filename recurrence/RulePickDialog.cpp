#include "RulePickDialog.h"

#include <QPushButton>

RulePickDialog::RulePickDialog(QWidget *parent, std::vector<std::string> &rules, unsigned int &currentRule) :
    RotatingDialog(parent),
    ui(new Ui::RulePickDialog),
    rules(rules),
    currentRule(currentRule)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    // Set up action buttins
    QPushButton *newButton    = new QPushButton(tr("New"));
    QPushButton *deleteButton = new QPushButton(tr("Delete"));
    QPushButton *doneButton   = new QPushButton(tr("Done"));
    ui->buttonBox->addButton(newButton,    QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(doneButton,   QDialogButtonBox::ActionRole);
    connect(newButton,    SIGNAL(clicked()), this, SLOT(newRule()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteRule()));
    connect(doneButton,   SIGNAL(clicked()), this, SLOT(accept()));

    // Add an item for each rule
    for (unsigned int r = 0; r < rules.size(); r++)
        QListWidgetItem *item = new QListWidgetItem(rules[r].c_str(), ui->ruleList);

    // Select the current rule and mark the row as current to prevent the
    // selection from disappearing when deleting the bottommost item.
    ui->ruleList->item(currentRule)->setSelected(true);
    ui->ruleList->setCurrentRow(currentRule);

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

RulePickDialog::~RulePickDialog()
{
    delete ui;
}

void RulePickDialog::resizeEvent(QResizeEvent *e)
{
    // Make sure that the current item is in view
    ui->ruleList->scrollToItem(ui->ruleList->currentItem(), QAbstractItemView::PositionAtCenter);

    RotatingDialog::resizeEvent(e);
}

void RulePickDialog::newRule()
{
    // Add and select an empty item
    QListWidgetItem *item = new QListWidgetItem(ui->ruleList);
    item->setSelected(true);

    // Save and exit
    accept();
}

void RulePickDialog::deleteRule()
{
    // Remove the selected item from the list
    QList<QListWidgetItem*> selection = ui->ruleList->selectedItems();
    if (!selection.isEmpty()) {
        delete selection.first();

        // Try to shrink the dialog
        ui->ruleList->updateGeometry();
        this->adjustSize();
    }
}

void RulePickDialog::accept()
{
    // Export the list of rules
    rules.clear();
    for (int i = 0; i < ui->ruleList->count(); i++)
        rules.push_back(ui->ruleList->item(i)->text().toAscii().data());

    // Export the index of the current rule
    QList<QListWidgetItem*> selection = ui->ruleList->selectedItems();
    currentRule = selection.isEmpty() ? 0 : ui->ruleList->row(selection.first());

    QDialog::accept();
}
