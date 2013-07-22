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

private:
    bool viewportEvent(QEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    QDate date;

    DateHeadingWidget *headingWidget;

private slots:
    void onContextMenuRequested(const QPoint &pos);

    void onComponentActivated(QListWidgetItem *item = NULL);

    void onScrolled(int position);

    void editCurrentComponent();
    void deleteCurrentComponent();
};

#endif // COMPONENTLISTWIDGET_H
