#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include "RotatingDialog.h"
#include "ui_ImportDialog.h"

class ImportDialog : public RotatingDialog
{
    Q_OBJECT

public:
    ImportDialog(QWidget *parent, QString filename);
    ~ImportDialog();

private:
    Ui::ImportDialog *ui;

    QString filename;
    int calendarId;

    int eventCount;
    int todoCount;
    int journalCount;

    bool duplicates;
    bool aborted;

    void closeEvent(QCloseEvent *e);

private slots:
    void infoStart();
    void infoStep();
    void importStart();
    void importStep();
};

#endif // IMPORTDIALOG_H
