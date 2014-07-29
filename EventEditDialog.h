#ifndef EVENTEDITDIALOG_H
#define EVENTEDITDIALOG_H

#include "ComponentEditDialog.h"
#include "ui_EventEditDialog.h"

#include <QDateTime>

#include <CEvent.h>

class EventEditDialog : public ComponentEditDialog
{
    Q_OBJECT

public:
    EventEditDialog(QWidget *parent, CEvent *event = NULL);
    ~EventEditDialog();

    void setFromTo(QDateTime from, QDateTime to = QDateTime());
    void setAllDay(bool allDay);

private:
    Ui::EventEditDialog *ui;

    CEvent *event;

    time_t duration;

    void updateAlarmReference();

private slots:
    void onAllDayChanged(bool enabled);
    void onFromChanged();
    void onToChanged();

    void saveEvent();
    void deleteEvent();
};

#endif // EVENTEDITDIALOG_H
