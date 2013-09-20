#ifndef BYDAYWIDGET_H
#define BYDAYWIDGET_H

#include <QFrame>

#include <QLabel>
#include <QTabWidget>

#include <QList>

#include "ByDayCoreWidget.h"

class ByDayWidget : public QFrame
{
    Q_OBJECT

public:
    ByDayWidget(QWidget *parent);

    void setMaximum(int maximum);

    QString rulePart() const;

    void parseRulePart(QString rule);
    void clear();

private:
    QLabel *titleLabel;
    QTabWidget *tabWidget;
    ByDayCoreWidget *positiveDaysWidget;
    ByDayCoreWidget *negativeDaysWidget;

    int maximum;
};

#endif // BYDAYWIDGET_H
