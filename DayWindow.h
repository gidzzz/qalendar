#include "ui_DayWindow.h"

#include <QDate>

#include <CComponent.h>

#include "ChangeClient.h"

class DayWindow : public QMainWindow, public ChangeClient
{
    Q_OBJECT

public:
    DayWindow(QDate date, QWidget *parent);
    ~DayWindow();

    void setDate(QDate date);

    void onChange();

private:
    Ui::DayWindow *ui;

    QDate date;

    vector<CComponent*> components;

    void cleanup();
    void reload();

    void changeEvent(QEvent *e);

private slots:
    void selectDay();
    void gotoPrevDay();
    void gotoNextDay();
    void gotoToday();
};
