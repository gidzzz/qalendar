#ifndef EXPANDINGLISTWIDGET_H
#define EXPANDINGLISTWIDGET_H

#include <QListWidget>

class ExpandingListWidget : public QListWidget
{
    Q_OBJECT

public:
    ExpandingListWidget(QWidget *parent) :
        QListWidget(parent)
    {
        this->setUniformItemSizes(true);

        this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    }

    QSize sizeHint() const
    {
        return QSize(256, qMax(1, this->count() * this->sizeHintForRow(0)));
    }
};

#endif // EXPANDINGLISTWIDGET_H
