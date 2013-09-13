#ifndef TUNEPICKDIALOG_H
#define TUNEPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_TunePickDialog.h"

namespace Ui {
    class TunePickDialog;
}

class TunePickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    TunePickDialog(QWidget *parent, QString currentPath);
    ~TunePickDialog();

signals:
    void selected(QString path);

private:
    Ui::TunePickDialog *ui;

    void addTune(QString path);

private slots:
    void accept();
};

#endif // TUNEPICKDIALOG_H
