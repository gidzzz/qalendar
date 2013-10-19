#include "CalendarPickDialog.h"

#include "Roles.h"

CalendarPickDialog::CalendarPickDialog(QWidget *parent, int currentId) :
    RotatingDialog(parent),
    ui(new Ui::CalendarPickDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    for (int i = 0; i < ui->calendarList->count(); i++) {
        if (ui->calendarList->item(i)->data(IdRole).toInt() == currentId) {
            ui->calendarList->setCurrentRow(i);
            break;
        }
    }

    connect(ui->calendarList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onCalendarActivated(QListWidgetItem*)));

    this->setFeatures(NULL, NULL);
}

CalendarPickDialog::~CalendarPickDialog()
{
    delete ui;
}

void CalendarPickDialog::resizeEvent(QResizeEvent *e)
{
    ui->calendarList->scrollToItem(ui->calendarList->currentItem(), QAbstractItemView::PositionAtCenter);

    RotatingDialog::resizeEvent(e);
}

void CalendarPickDialog::onCalendarActivated(QListWidgetItem *item)
{
    emit selected(item->data(IdRole).toInt());

    this->close();
}
