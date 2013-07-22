#ifndef COMPONENTEDITDIALOG_H
#define COMPONENTEDITDIALOG_H

#include "RotatingDialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

class ComponentEditDialog : public RotatingDialog
{
    Q_OBJECT

protected:
    ComponentEditDialog(QWidget *parent) :
        RotatingDialog(parent)
    {
        this->setAttribute(Qt::WA_DeleteOnClose);
    }

    void setupSaveButton(QDialogButtonBox *buttonBox, const char *slot)
    {
        QPushButton *saveButton = new QPushButton(tr("Save"));
        buttonBox->addButton(saveButton, QDialogButtonBox::ApplyRole);
        connect(saveButton, SIGNAL(clicked()), this, slot);
    }

    void setupDeleteButton(QDialogButtonBox *buttonBox, const char *slot)
    {
        QPushButton *deleteButton = new QPushButton(tr("Delete"));
        buttonBox->addButton(deleteButton, QDialogButtonBox::DestructiveRole);
        connect(deleteButton, SIGNAL(clicked()), this, slot);
    }
};

#endif // COMPONENTEDITDIALOG_H
