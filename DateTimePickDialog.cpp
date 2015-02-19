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
