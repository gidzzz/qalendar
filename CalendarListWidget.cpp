#include "CalendarListWidget.h"

#include <CMulticalendar.h>
#include <CCalendar.h>

#include "CalendarDelegate.h"

#include "CWrapper.h"
#include "Metrics.h"
#include "Roles.h"

CalendarListWidget::CalendarListWidget(QWidget *parent) :
    ExpandingListWidget(parent)
{
    this->setItemDelegate(new CalendarDelegate(this));

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();
    CWrapper::sort(calendars, true);

    // Add an item for each calendar found
    for (unsigned int c = 0; c < calendars.size(); c++) {
        using namespace Metrics::Item;

        // Do not show the birthday calendar
        if (calendars[c]->getCalendarType() == BIRTHDAY_CALENDAR) continue;

        QListWidgetItem *item = new QListWidgetItem();
        item->setData(IdRole, calendars[c]->getCalendarId());
        item->setData(NameRole, CWrapper::simplify(CWrapper::calendarName(calendars[c]->getCalendarName()).toUtf8().data(), TextMaxChars));
        item->setData(TypeRole, calendars[c]->getCalendarType());
        item->setData(ColorRole, calendars[c]->getCalendarColor());
        item->setData(VisibilityRole, calendars[c]->IsShown());

        this->addItem(item);
    }

    mc->releaseListCalendars(calendars);
}
