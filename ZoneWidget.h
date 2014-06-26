#ifndef TIMEZONEWIDGET_H
#define TIMEZONEWIDGET_H

#include <QMaemo5ValueButton>

class ZoneWidget : public QMaemo5ValueButton
{
    Q_OBJECT

public:
    ZoneWidget(QWidget *parent);

    QString currentZone();
    void setCurrentZone(QString zone);
};

#endif // TIMEZONEWIDGET_H
