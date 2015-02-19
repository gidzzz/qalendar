#ifndef TIMEPICKSELECTOR_H
#define TIMEPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

#include <QTime>

class TimePickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    TimePickSelector(QObject *parent = 0);

    QTime currentTime() const;
    void setCurrentTime(const QTime &time);

    QString currentValueText() const;

    QWidget* widget(QWidget *parent);

private:
    QTime time;

private slots:
    void onSelected(QTime time);
};

#endif // TIMEPICKSELECTOR_H
