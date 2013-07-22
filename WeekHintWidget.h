#ifndef WEEKHINTWIDGET_H
#define WEEKHINTWIDGET_H

#include <QWidget>

#include "WeekWidget.h"

class WeekHintWidget : public QWidget
{
    Q_OBJECT

public:
    enum Placement
    {
        Top,
        Bottom
    };

    WeekHintWidget(Placement placement, WeekWidget *weekWidget, QWidget *parent);

    void setPosition(int position);

private:
    WeekWidget *weekWidget;
    Placement placement;
    int position;

    void paintEvent(QPaintEvent *);
};

#endif // WEEKHINTWIDGET_H
