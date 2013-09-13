#ifndef TUNEPICKSELECTOR_H
#define TUNEPICKSELECTOR_H

#include <QMaemo5AbstractPickSelector>

class TunePickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT

public:
    TunePickSelector(QString path, QObject *parent = 0);

    QString currentValueText() const;
    QString currentPath() const;

    QWidget* widget(QWidget *parent);

    static QString nameForPath(QString path);

private:
    QString path;

private slots:
    void onSelected(QString path);
};

#endif // TUNEPICKSELECTOR_H
