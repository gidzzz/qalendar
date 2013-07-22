#ifndef BYWEEKDAYCOREWIDGET_H
#define BYWEEKDAYCOREWIDGET_H

#include <QWidget>

#include <QHBoxLayout>

class ByWeekdayCoreWidget : public QWidget
{
    Q_OBJECT

public:
    ByWeekdayCoreWidget(bool reverse, QWidget *parent);

    QString enabledRulePart() const;
    QString disabledRulePart() const;
    int findColumn(QString id);
    void parseDay(QString rule);

    void setEnabled(int day, bool visible);

private:
    QHBoxLayout *columnLayout;
};

#endif // BYWEEKDAYCOREWIDGET_H
