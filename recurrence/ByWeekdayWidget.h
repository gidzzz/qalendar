#ifndef BYWEEKDAYWIDGET_H
#define BYWEEKDAYWIDGET_H

#include <QFrame>

#include <QHBoxLayout>
#include <QTabWidget>

#include "ByWeekdayCoreWidget.h"

class ByWeekdayWidget : public QFrame
{
    Q_OBJECT

public:
    ByWeekdayWidget(QWidget *parent);

    QString rulePart() const;
    void parseRulePart(QString rule);
    void clear();

signals:
    void dayToggled(int day, bool checked);

private:
    QHBoxLayout *daysLayout;
    QTabWidget *tabWidget;
    ByWeekdayCoreWidget *positiveDaysWidget;
    ByWeekdayCoreWidget *negativeDaysWidget;

private slots:
    void onDayToggled(bool checked);
};

#endif // BYWEEKDAYWIDGET_H
