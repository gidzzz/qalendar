#ifndef ZONEPICKSELECTOR_H
#define ZONEPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

class ZonePickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    ZonePickSelector(QObject *parent = 0);

    QWidget* widget(QWidget *parent);

    QString currentValueText() const;
    QString currentZone() const;

public slots:
    void setCurrentZone(const QString &zone);

private:
    QString zone;
};

#endif // ZONEPICKSELECTOR_H
