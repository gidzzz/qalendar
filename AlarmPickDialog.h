#ifndef ALARMPICKDIALOG_H
#define ALARMPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_AlarmPickDialog.h"

namespace Ui {
    class AlarmPickDialog;
}

class AlarmPickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    AlarmPickDialog(int seconds, QWidget *parent);
    ~AlarmPickDialog();

signals:
    void selected(int seconds);

public slots:
    void accept();

private:
    Ui::AlarmPickDialog *ui;
};

#endif // ALARMPICKDIALOG_H
