#include "CalendarPickSelector.h"

#include <QMaemo5ValueButton>

#include <CMulticalendar.h>

#include "CalendarPickDialog.h"

#include "CWrapper.h"

CalendarPickSelector::CalendarPickSelector(QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
    // This is the ID of the default calendar, so it should always be valid
    setCalendar(1);
}

void CalendarPickSelector::setCalendar(int calendarId)
{
    int error = 0;
    CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(calendarId, error);

    if (calendar) {
        currentCalendarId = calendarId;
        currentCalendarName = CWrapper::calendarName(calendar->getCalendarName());

        QMaemo5ValueButton *button = qobject_cast<QMaemo5ValueButton*>(this->parent());

        if (button)
            button->setIcon(QIcon::fromTheme(CWrapper::colorIcon(calendar->getCalendarColor())));

        delete calendar;

        emit selected(currentCalendarName);
    }
}

QString CalendarPickSelector::currentValueText() const
{
    return currentCalendarName;
}

int CalendarPickSelector::currentId() const
{
    return currentCalendarId;
}

QWidget* CalendarPickSelector::widget(QWidget *parent)
{
    CalendarPickDialog *dialog = new CalendarPickDialog(parent, currentId());

    connect(dialog, SIGNAL(selected(int)), this, SLOT(onSelected(int)));

    return dialog;
}

void CalendarPickSelector::onSelected(int id)
{
    setCalendar(id);
}
