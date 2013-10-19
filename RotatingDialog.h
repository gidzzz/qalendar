#ifndef ROTATINGDIALOG_H
#define ROTATINGDIALOG_H

#include <QDialog>

#include <QGridLayout>

#include "Rotator.h"

class RotatingDialog : public QDialog
{
    Q_OBJECT

    QGridLayout *dialogLayout;
    QWidget *buttonBox;

protected:
    RotatingDialog(QWidget *parent) :
        QDialog(parent),
        dialogLayout(NULL),
        buttonBox(NULL)
    {
    }

    void setFeatures(QGridLayout *dialogLayout, QWidget *buttonBox)
    {
        this->dialogLayout = dialogLayout;
        this->buttonBox = buttonBox;

        Rotator *rotator = Rotator::acquire();
        connect(rotator, SIGNAL(rotated(int,int)), this, SLOT(onRotated(int,int)));
        onRotated(rotator->width(), rotator->height());
    }

private slots:
    void onRotated(int w, int h)
    {
        if (dialogLayout && buttonBox) {
            dialogLayout->removeWidget(buttonBox);

            // QGridLayout::row-/columnCount() does not return what could
            // be expected of it. Calculate the real dimensions manually.
            int rowCount = 0;
            int colCount = 0;
            for (int i = 0; i < dialogLayout->count(); i++) {
                int row, rowSpan;
                int col, colSpan;
                dialogLayout->getItemPosition(i, &row, &col, &rowSpan, &colSpan);
                rowCount = qMax(rowCount, row + rowSpan);
                colCount = qMax(colCount, col + colSpan);
            }

            if (w < h) { // Portrait
                dialogLayout->addWidget(buttonBox, rowCount, 0, 1, colCount);
                buttonBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                buttonBox->setContentsMargins(0, 0, 0, 0);
                buttonBox->layout()->setSpacing(0);
                dialogLayout->setContentsMargins(5, 0, 5, 0);
            } else { // Landscape
                dialogLayout->addWidget(buttonBox, 0, colCount, rowCount, 1);
                buttonBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
                buttonBox->setContentsMargins(0, 5, 0, 5);
                buttonBox->layout()->setSpacing(5);
                dialogLayout->setContentsMargins(15, 0, 15, 0);
            }
        }

        this->adjustSize();
    }
};

#endif // ROTATINGDIALOG_H
