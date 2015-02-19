#ifndef SCROLLER_H
#define SCROLLER_H

#include <QListWidget>

class Scroller
{
public:
    const int width;

    Scroller(QListWidget *listWidget, int width, int min) :
        width(width), listWidget(listWidget), min(min)
    {
        // Prevent the managed list from stealing focus
        listWidget->setFocusPolicy(Qt::NoFocus);
    }

    void scroll(int row)
    {
        listWidget->setCurrentRow(qBound(0, row - min, listWidget->count()-1));
    }

private:
    QListWidget *listWidget;
    int min;
};

#endif // SCROLLER_H
