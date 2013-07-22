#ifndef CHAINEDSPINBOX_H
#define CHAINEDSPINBOX_H

#include <QSpinBox>

// TODO: Keybard shortcut to jump between boxes

class ChainedSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    ChainedSpinBox(QWidget *parent) :
        QSpinBox(parent),
        successor(NULL)
    {
        this->setAlignment(Qt::AlignCenter);
    }

    void setSuccessor(ChainedSpinBox *successor)
    {
        this->successor = successor;
    }

    StepEnabled stepEnabled() const
    {
        StepEnabled status = StepNone;

        if (this->value() > this->minimum()
        ||  successor && successor->stepEnabled() & StepDownEnabled)
        {
            status |= StepDownEnabled;
        }

        if (this->value() < this->maximum()
        ||  successor && successor->stepEnabled() & StepUpEnabled)
        {
            status |= StepUpEnabled;
        }

        return status;
    }

    void stepBy(int steps)
    {
        int newValue = this->value() + steps;

        if (successor) {
            if (steps > 0) {
                if (newValue > this->maximum() && successor->stepEnabled() & StepUpEnabled) {
                    successor->stepBy(1);
                    newValue = this->minimum();
                }
            } else if (steps < 0){
                if (newValue < this->minimum() && successor->stepEnabled() & StepDownEnabled) {
                    successor->stepBy(-1);
                    newValue = this->maximum();
                }
            }
        }

        this->setValue(qBound(this->minimum(), newValue, this->maximum()));
    }

    QString textFromValue(int value) const
    {
        return QString::number(value).rightJustified(2, '0');
    }

private:
    ChainedSpinBox *successor;
};

#endif // CHAINEDSPINBOX_H
