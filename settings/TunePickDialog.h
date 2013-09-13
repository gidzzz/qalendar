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

    QListWidgetItem *customItem;

    void addTune(QString path, bool front = false);

private slots:
    void browse();
    void accept();
};

#endif // TUNEPICKDIALOG_H
