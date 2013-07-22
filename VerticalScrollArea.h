#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H

#include <QScrollArea>

#include <QResizeEvent>

class VerticalScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    VerticalScrollArea(QWidget *parent) :
        QScrollArea(parent)
    {
        this->setWidgetResizable(true);
    }

protected:
    bool viewportEvent(QEvent *e)
    {
        if (e->type() == QEvent::Resize)
            this->widget()->setFixedWidth(static_cast<QResizeEvent*>(e)->size().width());

        return QScrollArea::viewportEvent(e);
    }
};

#endif // VERTICALSCROLLAREA_H
