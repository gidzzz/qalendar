#ifndef BYWEEKDAYCOLUMNWIDGET_H
#define BYWEEKDAYCOLUMNWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include <QList>

class ByWeekdayColumnWidget : public QWidget
{
    Q_OBJECT

public:
    ByWeekdayColumnWidget(QString weekdayId, bool reverse, QWidget *parent);

    QString id() const;
    QString rulePart() const;

    void addDay(int day);

public slots:
    void setEnabled(bool enable);

private:
    QVBoxLayout *listLayout;

    QString weekdayId;
    bool reverse;

    QList<int> days;

private slots:
    void showAddDialog();
    void onDayClicked();
};

#endif // BYWEEKDAYCOLUMNWIDGET_H
