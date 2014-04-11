#ifndef BIRTHDAYEDITDIALOG_H
#define BIRTHDAYEDITDIALOG_H

#include "ComponentEditDialog.h"
#include "ui_BirthdayEditDialog.h"

#include <CBdayEvent.h>

class BirthdayEditDialog : public ComponentEditDialog
{
    Q_OBJECT

public:
    BirthdayEditDialog(QWidget *parent, CBdayEvent *event);
    ~BirthdayEditDialog();

private:
    Ui::BirthdayEditDialog *ui;

    CBdayEvent *event;

private slots:
    void saveEvent();
};

#endif // BIRTHDAYEDITDIALOG_H
