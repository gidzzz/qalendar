#ifndef NEWCOMPONENTWIDGET_H
#define NEWCOMPONENTWIDGET_H

#include <QWidget>

#include <QPushButton>
#include <QHBoxLayout>

class NewComponentWidget : public QWidget
{
    Q_OBJECT

public:
    QPushButton *eventButton;
    QPushButton *todoButton;

    NewComponentWidget()
    {
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        todoButton  = new QPushButton(QIcon::fromTheme("calendar_todo"), QString(), this);
        eventButton = new QPushButton(QIcon::fromTheme("general_add"), tr("New event"), this);
        eventButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        mainLayout->addWidget(todoButton);
        mainLayout->addWidget(eventButton);
    }
};

#endif // NEWCOMPONENTWIDGET_H
