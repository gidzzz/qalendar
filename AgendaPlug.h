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

    void onActivated();

    void setDate(QDate date);

private:
    Ui::AgendaPlug *ui;

    QDate date;

    vector<CComponent*> components;

    void cleanup();
    void reload();

private slots:
    void selectDay();
    void gotoToday();
};
