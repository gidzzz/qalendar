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
    void contextMenuEvent(QContextMenuEvent *e);

private slots:
    void gotoPrevEvent();
    void gotoNextEvent();

    void editEvent();
    void cloneEvent();
    void deleteEvent();

    void enableLinks(bool enalbe);
};

#endif // EVENTWINDOW_H
