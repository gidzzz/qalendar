#ifndef BYMONTHWIDGET_H
#define BYMONTHWIDGET_H

#include <QFrame>

#include <QGridLayout>

class ByMonthWidget : public QFrame
{
    Q_OBJECT

public:
    ByMonthWidget(QWidget *parent);

    QString rulePart() const;
    void parseRulePart(QString rule);
    void clear();

private:
    QGridLayout *monthsLayout;
};

#endif // BYMONTHWIDGET_H
