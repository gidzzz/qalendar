#include "DateTimePickDialog.h"

#include <QPushButton>
#include <QScrollBar>
#include <QAbstractKineticScroller>

DateTimePickDialog::DateTimePickDialog(QWidget *parent) :
    RotatingDialog(parent),
    ui(new Ui::DateTimePickDialog),
    resizeCount(0)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(new QPushButton(tr("Done")), QDialogButtonBox::AcceptRole);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

void DateTimePickDialog::keyPressEvent(QKeyEvent *e)
{
    // Try to map virtual keys to digits
    int digit = e->nativeVirtualKey() - '0';
    if (digit < 0 || digit > 9) {
        // Try to map top row keys to digits
        digit = e->nativeScanCode() - 24;
        if (digit < 0 || digit > 9) {
            // Failure, abort
            RotatingDialog::keyPressEvent(e);
            return;
        }
        digit = (digit + 1) % 10;
    }

    // Calculate maximum input length
    int scrollersInputLength = 0;
    for (int i = 0; i < scrollers.size(); i++)
        scrollersInputLength += scrollers[i].width;

    // Register the input
    scrollersInput.append(digit);
    if (scrollersInput.size() > scrollersInputLength)
        scrollersInput.removeFirst();

    // Forward input parts to appropriate scrollers
    for (int s = 0, d = 0; s < scrollers.size() && scrollersInput.size() - d >= scrollers[s].width; d += scrollers[s].width, s++) {
        int inputPart = 0;
        for (int i = 0; i < scrollers[s].width; i++)
            inputPart = inputPart * 10 + scrollersInput[d + i];

        scrollers[s].scroll(inputPart);
    }

    centerView();
}

void DateTimePickDialog::resizeEvent(QResizeEvent *e)
{
    // HACK: Make the scroll-to-item work by waiting for the lists to be properly resized before scrolling
    if (++resizeCount == 2)
        centerView();

    QDialog::resizeEvent(e);
}

int DateTimePickDialog::row(QListWidget *listWidget)
{
    const QList<QListWidgetItem*> selection = listWidget->selectedItems();

    return selection.isEmpty() ? -1 : listWidget->row(selection.first());
}

void DateTimePickDialog::centerView(QListWidget *listWidget)
{
    const int itemHeight = listWidget->visualItemRect(listWidget->item(0)).height();

    listWidget->property("kineticScroller").value<QAbstractKineticScroller*>()
              ->scrollTo(QPoint(0, qBound(0,
                                          row(listWidget) * itemHeight + (itemHeight - listWidget->height()) / 2,
                                          listWidget->verticalScrollBar()->maximum())));
}
