#ifndef DATEPICKSELECTOR_H
#define DATEPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

#include <QDate>

class DatePickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    DatePickSelector(QObject *parent = 0);

    QDate currentDate() const;
    void setCurrentDate(const QDate &date);

    QString currentValueText() const;

    QWidget* widget(QWidget *parent);

private:
    QDate date;

private slots:
    void onSelected(QDate date);
};

#endif // DATEPICKSELECTOR_H
