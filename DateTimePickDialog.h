#ifndef DATETIMEPICKDIALOG_H
#define DATETIMEPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_DateTimePickDialog.h"

#include <QListWidget>

class DateTimePickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    DateTimePickDialog(QWidget *parent);

protected:
    Ui::DateTimePickDialog *ui;

    void resizeEvent(QResizeEvent *e);

    int row(QListWidget *listWidget);
    void centerView(QListWidget *listWidget);

    virtual void centerView() = 0;

private:
    int resizeCount;
};

#endif // DATETIMEPICKDIALOG_H
