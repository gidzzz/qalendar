#ifndef EVENTWINDOW_H
#define EVENTWINDOW_H

#include "ui_EventWindow.h"

#include "ChangeClient.h"

#include "ComponentInstance.h"

class EventWindow : public QMainWindow, public ChangeClient
{
    Q_OBJECT

public:
    EventWindow(ComponentInstance *instance, QWidget *parent);
    ~EventWindow();

    void onChange();

private:
    Ui::EventWindow *ui;

    ComponentInstance instance;

    void reload();

    void changeEvent(QEvent *e);

private slots:
    void gotoPrevEvent();
    void gotoNextEvent();

    void editEvent();
    void deleteEvent();
};

#endif // EVENTWINDOW_H
