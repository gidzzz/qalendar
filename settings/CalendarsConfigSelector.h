#ifndef CALENDARSCONFIGSELECTOR_H
#define CALENDARSCONFIGSELECTOR_H

#include <QMaemo5AbstractPickSelector>

class CalendarsConfigSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    CalendarsConfigSelector(QObject *parent = 0);

    QString currentValueText() const;

    QWidget* widget(QWidget *parent);

private slots:
    void reload();
};

#endif // CALENDARSCONFIGSELECTOR_H
