#ifndef RECURRENCEPICKSELECTOR_H
#define RECURRENCEPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

#include <CRecurrence.h>

class RecurrencePickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    RecurrencePickSelector(QObject *parent = 0);

    QString currentValueText() const;
    CRecurrence *currentRecurrence() const;

    QWidget* widget(QWidget *parent);

    void setRecurrence(CRecurrence *recurrence);

private:
    CRecurrence *recurrence;

private slots:
    void onSelected();
};

#endif // RECURRENCEPICKSELECTOR_H
