#ifndef MONTHWIDGET_H
#define MONTHWIDGET_H

#include <QWidget>

#include <QDate>
#include <QDateTime>
#include <QMouseEvent>
#include <QPixmap>

#include <CComponent.h>

#include <vector>
#include <map>

#include "ComponentInstance.h"

namespace Metrics
{
    namespace MonthWidget
    {
        const int CellWidth  = 102;
        const int CellHeight = 66;
        const int NumWeeks = 6;
        const int NumWeekdays = 7;
    }
}

class MonthWidget : public QWidget
{
    Q_OBJECT

public:
    MonthWidget(QDate date, QWidget *parent);

    void setDate(QDate date);
    QDate firstDate();
    QDate lastDate();

signals:
    void swipedPrev();
    void swipedNext();
    void swipedPrevFar();
    void swipedNextFar();

protected:
    QDate date;

    void reload();

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    void forgetPress();

    QDate pressedDate;
    QPoint pressedPoint;

    QDate mapToDate(const QPoint &pos);

    QPixmap render();

    vector<CComponent*> components;
    vector<ComponentInstance*> instances;
    map<int, int> palette;
};

#endif // MONTHWIDGET_H
