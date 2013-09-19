#ifndef RECURRENCEPICKDIALOG_H
#define RECURRENCEPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_RecurrencePickDialog.h"

#include <CRecurrence.h>

class RecurrencePickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    RecurrencePickDialog(QWidget *parent, CRecurrence *recurrence);
    ~RecurrencePickDialog();

public slots:
    void accept();

private:
    Ui::RecurrencePickDialog *ui;

    CRecurrence *recurrence;

    static QString freqToStr(int frequency);
    static int freqFromStr(QString frequency);
    static int recurrenceType(CRecurrence *recurrence);

    void parseRule(QString rule);

private slots:
    void onFrequencyChanged(int index);
    void onLimitTypeChanged(int index);
};

#endif // RECURRENCEPICKDIALOG_H
