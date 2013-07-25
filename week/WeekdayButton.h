#ifndef WEEKDAYBUTTON_H
#define WEEKDAYBUTTON_H

#include <QAbstractButton>

#include <QDate>

class WeekdayButton : public QAbstractButton
{
    Q_OBJECT

public:
    WeekdayButton(QWidget *parent);

    QSize sizeHint() const;

    void setDate(QDate date);

private:
    QDate date;

    void paintEvent(QPaintEvent *);

private slots:
    void onClicked();
};

#endif // WEEKDAYBUTTON_H
