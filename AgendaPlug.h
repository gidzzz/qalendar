#ifndef AGENDAPLUG_H
#define AGENDAPLUG_H

#include "ui_AgendaPlug.h"

#include <QDate>

#include <CComponent.h>

#include "TemporalPlug.h"

class AgendaPlug : public TemporalPlug
{
    Q_OBJECT

public:
    AgendaPlug(QDate date, QWidget *parent);
    ~AgendaPlug();

    QString title() const { return tr("Agenda"); }
    bool isRotatable() const { return true; }

    void onChange();

    void setDate(QDate date, bool align = true);

private:
    Ui::AgendaPlug *ui;

    vector<CComponent*> components;

    void cleanup();
    void reload(bool align);

private slots:
    void selectDay();
    void gotoToday();
};

#endif // AGENDAPLUG_H
