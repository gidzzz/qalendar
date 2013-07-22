#ifndef DAYNUMBERDIALOG_H
#define DAYNUMBERDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QPushButton>

class DayNumberDialog : public QDialog
{
    Q_OBJECT

public:
    DayNumberDialog(int maximum, QWidget *parent) :
        QDialog(parent)
    {
        this->setWindowTitle("Choose day number");

        QHBoxLayout *mainLayout = new QHBoxLayout(this);

        numberBox = new QSpinBox(this);
        numberBox->setRange(1, maximum);

        QPushButton *acceptButton = new QPushButton("Add", this);

        mainLayout->addWidget(numberBox);
        mainLayout->addWidget(acceptButton);

        connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));
    }

    int value()
    {
        return numberBox->value();
    }

private:
    QSpinBox *numberBox;
};

#endif // DAYNUMBERDIALOG_H
