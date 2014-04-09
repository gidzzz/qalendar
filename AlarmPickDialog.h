#ifndef ALARMPICKDIALOG_H
#define ALARMPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_AlarmPickDialog.h"

class AlarmPickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    AlarmPickDialog(int type, int beforeTime, int triggerTime, QWidget *parent);
    ~AlarmPickDialog();

signals:
    void selected(bool enabled, int type, int beforeTime, int triggerTime);

public slots:
    void accept();

private:
    Ui::AlarmPickDialog *ui;

private slots:
    void onTypeButtonClicked(int type);
};

#endif // ALARMPICKDIALOG_H
