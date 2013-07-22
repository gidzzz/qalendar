#ifndef COLORPICKDILOG_H
#define COLORPICKDILOG_H

#include "RotatingDialog.h"
#include "ui_ColorPickDialog.h"

#include <QButtonGroup>

#include <CCalendar.h>

namespace Ui {
    class ColorPickDialog;
}

class ColorPickDialog : public RotatingDialog
{
    Q_OBJECT

public:
    ColorPickDialog(int color, QWidget *parent);
    ~ColorPickDialog();

signals:
    void selected(int color);

public slots:
    void accept();

private:
    Ui::ColorPickDialog *ui;

    QButtonGroup *colorGroup;
};

#endif // COLORPICKDILOG_H
