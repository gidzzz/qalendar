#ifndef EVENTEDITDIALOG_H
#define EVENTEDITDIALOG_H

#include "ui_EventEditDialog.h"

#include <ComponentEditDialog.h>

#include <QDateTime>

#include <CEvent.h>

namespace Ui {
    class EventEditDialog;
}

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

private slots:
    void onFromChanged();
    void onToChanged();

    void saveEvent();
    void deleteEvent();
};

#endif // EVENTEDITDIALOG_H
