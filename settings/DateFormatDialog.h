#ifndef DATEFORMATDIALOG_H
#define DATEFORMATDIALOG_H

#include "RotatingDialog.h"
#include "ui_DateFormatDialog.h"

class DateFormatDialog : public RotatingDialog
{
    Q_OBJECT

public:
    DateFormatDialog(QWidget *parent);
    ~DateFormatDialog();

public slots:
    void accept();

private:
    Ui::DateFormatDialog *ui;
};

#endif // DATEFORMATDIALOG_H
