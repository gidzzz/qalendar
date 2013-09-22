#ifndef CALENDARPICKSELECTOR_H
#define CALENDARPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

class CalendarPickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    CalendarPickSelector(QObject *parent = 0);

    QString currentValueText() const;
    int currentId() const;

    QWidget* widget(QWidget *parent);

    void setCalendar(int calendarId);

private:
    QString currentCalendarName;
    int currentCalendarId;

private slots:
    void onSelected(int id);
};

#endif // CALENDARPICKSELECTOR_H
