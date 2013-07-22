#ifndef DATEHEADINGWIDGET_H
#define DATEHEADINGWIDGET_H

#include <QWidget>
#include <QMaemo5Style>
#include <QPainter>
#include <QDate>

class DateHeadingWidget : public QWidget
{
    Q_OBJECT

public:
    DateHeadingWidget(QWidget *parent) :
        QWidget(parent)
    {
    }

    void setDate(QDate date)
    {
        this->date = date;
    }

    static void paint(QPainter &painter, const QRect &rect, const QDate &date)
    {
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(13);
        painter.setFont(font);

        painter.setPen(QMaemo5Style::standardColor("SecondaryTextColor"));

        painter.fillRect(rect, QPalette().color(QPalette::Window));

        painter.drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter,
                         date.toString("dddd d MMMM yyyy"));
    }

private:
    QDate date;

    void paintEvent(QPaintEvent *)
    {
        QPainter painter(this);

        paint(painter, this->rect(), date);
    }
};

#endif // DATEHEADINGWIDGET_H
