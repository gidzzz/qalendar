#ifndef ALARMPICKSELECTOR_H
#define ALARMPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

class AlarmPickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    AlarmPickSelector(QObject *parent = 0);

    QString currentValueText() const;
    int currentSecondsBefore() const;

    QWidget* widget(QWidget *parent);

    void setSecondsBefore(int seconds);

    static QString textForSeconds(int seconds);

private:
    int secondsBefore;

private slots:
    void onSelected(int seconds);
};

#endif // ALARMPICKSELECTOR_H
