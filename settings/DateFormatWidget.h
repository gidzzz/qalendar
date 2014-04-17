#ifndef DATEFORMATWIDGET_H
#define DATEFORMATWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

#include "Date.h"

class DateFormatWidget : public QWidget
{
    Q_OBJECT

public:
    DateFormatWidget(QWidget *parent);

    void setType(Date::Format type);

    QString format();

private:
    Date::Format type;

    QLineEdit *formatEdit;
    QLabel *previewLabel;

private slots:
    void onFormatChanged(QString format);
};

#endif // DATEFORMATWIDGET_H
