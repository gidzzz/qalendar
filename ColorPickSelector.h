#ifndef COLORPICKSELECTOR_H
#define COLORPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

#include <ColorPickDialog.h>

class ColorPickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    ColorPickSelector(QObject *parent = 0);

    QString currentValueText() const;
    int currentColor() const;

    QWidget* widget(QWidget *parent);

    void setColor(int color);

private:
    int color;

private slots:
    void onSelected(int color);
};

#endif // COLORPICKSELECTOR_H
