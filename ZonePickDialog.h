#ifndef ZONEPICKDIALOG_H
#define ZONEPICKDIALOG_H

#include "RotatingDialog.h"
#include "ui_ZonePickDialog.h"

#include <QKeyEvent>

class ZonePickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    ZonePickDialog(QWidget *parent, const QString &zone);
    ~ZonePickDialog();

    static QString displayName(const QString &zone);
    static QString displayOffset(int offset);

signals:
    void selected(QString zone);

private:
    Ui::ZonePickDialog *ui;

    void resizeEvent(QResizeEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private slots:
    void onDefaultClicked();
    void onZoneActivated(QListWidgetItem *item);
    void onSearchTextChanged(const QString &text);
};

#endif // ZONEPICKDIALOG_H
