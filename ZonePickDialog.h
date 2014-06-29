#ifndef ZONEPICKDIALOG_H
#define ZONEPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_ZonePickDialog.h"

class ZonePickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    ZonePickDialog(QWidget *parent, const QString &zone);
    ~ZonePickDialog();

    static QString displayName(QString zone);

signals:
    void selected(QString zone);

private:
    Ui::ZonePickDialog *ui;

    void resizeEvent(QResizeEvent *e);

private slots:
    void onZoneActivated(QListWidgetItem *item);
};

#endif // ZONEPICKDIALOG_H
