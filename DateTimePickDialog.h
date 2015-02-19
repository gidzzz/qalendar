#ifndef DATETIMEPICKDIALOG_H
#define DATETIMEPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_DateTimePickDialog.h"

#include <QListWidget>
#include <QKeyEvent>

#include "Scroller.h"

class DateTimePickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    DateTimePickDialog(QWidget *parent);

protected:
    Ui::DateTimePickDialog *ui;

    QList<Scroller> scrollers;

    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);

    int row(QListWidget *listWidget);
    void centerView(QListWidget *listWidget);

    virtual void centerView() = 0;

private:
    QList<int> scrollersInput;

    int resizeCount;
};

#endif // DATETIMEPICKDIALOG_H
