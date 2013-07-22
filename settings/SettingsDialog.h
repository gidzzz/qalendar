#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "RotatingDialog.h"
#include "ui_SettingsDialog.h"

#include <QMaemo5ListPickSelector>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public RotatingDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;

    QMaemo5ListPickSelector* buildDeleteSelector(int selectValue);

private slots:
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
