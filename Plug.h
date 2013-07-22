#ifndef PLUG_H
#define PLUG_H

#include <QWidget>

#include "ChangeClient.h"

class Plug : public QWidget, public ChangeClient
{
    Q_OBJECT

public:
    Plug(QWidget *parent) : QWidget(parent) { }

    QList<QAction*> actions;

    virtual QString title() const = 0;
    virtual bool isRotatable() const { return false; };

    virtual void onActivated() { };

signals:
    void titleChanged(QString title);
};

#endif // PLUG_H
