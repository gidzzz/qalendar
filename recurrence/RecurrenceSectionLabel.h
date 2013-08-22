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
        const QColor  enabledColor = QMaemo5Style::standardColor("ActiveTextColor");
        const QColor disabledColor = QMaemo5Style::standardColor("DisabledTextColor");
        palette.setColor(QPalette::Active,   QPalette::Foreground, enabledColor);
        palette.setColor(QPalette::Inactive, QPalette::Foreground, enabledColor);
        palette.setColor(QPalette::Disabled, QPalette::Foreground, disabledColor);
        this->setPalette(palette);

        this->setIndent(5);
    }
};

#endif // RECURRENCESECTIONLABEL_H
