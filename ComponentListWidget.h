#ifndef COMPONENTLISTWIDGET_H
#define COMPONENTLISTWIDGET_H

#include <QListWidget>

#include <QDate>

#include "DateHeadingWidget.h"

class ComponentListWidget : public QListWidget
{
    Q_OBJECT

public:
    ComponentListWidget(QWidget *parent);

    void setFloatingHeadings(bool enable);

    void setDate(QDate date);

public slots:
    void newEvent();
    void newTodo();

private:
    bool viewportEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    QDate date;

    DateHeadingWidget *headingWidget;

private slots:
    void onContextMenuRequested(const QPoint &pos);
    void onComponentActivated(QListWidgetItem *item);

    void onScrolled(int position);

    void editCurrentComponent();
    void cloneCurrentComponent();
    void deleteCurrentComponent();
};

#endif // COMPONENTLISTWIDGET_H
