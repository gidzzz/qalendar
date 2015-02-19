#ifndef TIMEPICKDIALOG_H
#define TIMEPICKDIALOG_H

#include "DateTimePickDialog.h"

#include <QListWidget>

#include <QTime>

class TimePickDialog : public DateTimePickDialog
{
    Q_OBJECT

    using DateTimePickDialog::centerView;

public:
    TimePickDialog(QTime time, QWidget *parent);

    QTime time();

signals:
    void selected(QTime time);

private:
    QListWidget *hList;
    QListWidget *mList;

    void setTime(const QTime &time);

private slots:
    void centerView();

    void reset();
    void accept();
};

#endif // TIMEPICKDIALOG_H
