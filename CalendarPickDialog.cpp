#include "CalendarPickDialog.h"

#include <vector>
#include <CMulticalendar.h>
#include <CCalendar.h>

#include "CWrapper.h"
#include "Metrics.h"
#include "Roles.h"

#include "CalendarDelegate.h"

CalendarPickDialog::CalendarPickDialog(QWidget *parent, int currentId) :
    RotatingDialog(parent),
    ui(new Ui::CalendarPickDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->calendarList->setItemDelegate(new CalendarDelegate(ui->calendarList));

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();
    CWrapper::sort(calendars);

    // Add an item for each calendar found
    for (unsigned int c = 0; c < calendars.size(); c++) {
        using namespace Metrics::Item;

        // Do not show the birthday calendar
        if (calendars[c]->getCalendarType() == BIRTHDAY_CALENDAR) continue;

        QListWidgetItem *item = new QListWidgetItem();
        item->setData(IdRole, calendars[c]->getCalendarId());
        item->setData(NameRole, CWrapper::simplify(calendars[c]->getCalendarName(), TextMaxChars));
        item->setData(TypeRole, calendars[c]->getCalendarType());
        item->setData(ColorRole, calendars[c]->getCalendarColor());

        ui->calendarList->addItem(item);

        if (calendars[c]->getCalendarId() == currentId)
            ui->calendarList->setCurrentItem(item);
    }

    mc->releaseListCalendars(calendars);

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
