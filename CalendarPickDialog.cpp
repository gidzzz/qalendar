#include "CalendarPickDialog.h"

#include <QScrollBar>

#include <vector>
#include <CMulticalendar.h>
#include <CCalendar.h>

#include "CWrapper.h"
#include "Roles.h"

#include "CalendarDelegate.h"

CalendarPickDialog::CalendarPickDialog(QWidget *parent) :
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
        // Do not show the birthday calendar
        if (calendars[c]->getCalendarType() == BIRTHDAY_CALENDAR) continue;

        QListWidgetItem *item = new QListWidgetItem();
        item->setData(IdRole, calendars[c]->getCalendarId());
        item->setData(NameRole, QString::fromUtf8(calendars[c]->getCalendarName().c_str()));
        item->setData(TypeRole, calendars[c]->getCalendarType());
        item->setData(ColorRole, calendars[c]->getCalendarColor());

        ui->calendarList->addItem(item);
    }

    mc->releaseListCalendars(calendars);

    connect(ui->calendarList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onCalendarActivated(QListWidgetItem*)));

    this->setFeatures(NULL, NULL);
}

CalendarPickDialog::~CalendarPickDialog()
{
    delete ui;
}

void CalendarPickDialog::onCalendarActivated(QListWidgetItem *item)
{
    emit selected(item->data(IdRole).toInt());

    this->close();
}
