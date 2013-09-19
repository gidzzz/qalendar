#ifndef JOURNALEDITDIALOG_H
#define JOURNALEDITDIALOG_H

#include "ComponentEditDialog.h"
#include "ui_JournalEditDialog.h"

#include <CJournal.h>

class JournalEditDialog : public ComponentEditDialog
{
    Q_OBJECT

public:
    JournalEditDialog(QWidget *parent, CJournal *journal = NULL);
    ~JournalEditDialog();

private:
    Ui::JournalEditDialog *ui;

    CJournal *journal;

private slots:
    void saveJournal();
    void deleteJournal();
};

#endif // JOURNALEDITDIALOG_H
