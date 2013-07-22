#ifndef WEEKAREA_H
#define WEEKAREA_H

#include <QScrollArea>

#include "WeekWidget.h"
#include "WeekHintWidget.h"

class WeekArea : public QScrollArea
{
    Q_OBJECT

public:
    WeekArea(QWidget *parent);

    WeekWidget* widget();

private:
    WeekHintWidget *topHintWidget;
    WeekHintWidget *bottomHintWidget;

    bool viewportEvent(QEvent *e);

private slots:
    void onScrolled(int position);
};

#endif // WEEKAREA_H
