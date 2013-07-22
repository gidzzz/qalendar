#ifndef RECURRENCESECTIONLABEL_H
#define RECURRENCESECTIONLABEL_H

#include <QLabel>

#include <QMaemo5Style>

class RecurrenceSectionLabel : public QLabel
{
    Q_OBJECT

public:
    RecurrenceSectionLabel(QWidget *parent = 0) :
        QLabel(parent)
    {
        init();
    }

    RecurrenceSectionLabel(QString text, QWidget *parent = 0) :
        QLabel(text, parent)
    {
        init();
    }

private:
    void init()
    {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Foreground, QMaemo5Style::standardColor("ActiveTextColor"));
        this->setPalette(palette);

        this->setIndent(5);
    }
};

#endif // RECURRENCESECTIONLABEL_H
