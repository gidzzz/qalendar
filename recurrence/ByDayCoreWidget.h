#ifndef BYDAYCOREWIDGET_H
#define BYDAYCOREWIDGET_H

#include <QWidget>
#include <QGridLayout>

class ByDayCoreWidget : public QWidget
{
    Q_OBJECT

public:
    ByDayCoreWidget(bool reverse, QWidget *parent);

    void setMaximum(int maximum);

    QList<int> selectedDays();
    void selectDays(QList<int> days);

private:
    int maximum;
    bool reverse;

    QGridLayout *listLayout;

    QList<int> days;
    void addDay(int day);
    void updateButtons();

    QPoint mapToGrid(int index) const;

private slots:
    void showAddDialog();
    void onDayClicked();
};

#endif // BYDAYCOREWIDGET_H
