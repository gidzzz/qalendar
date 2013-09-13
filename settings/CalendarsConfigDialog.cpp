#include "CalendarsConfigDialog.h"

#include <CMulticalendar.h>
#include <CCalendar.h>

#include "CWrapper.h"
#include "Metrics.h"

#include "DetailedCalendarDelegate.h"

#include "CalendarEditDialog.h"

#include "ChangeManager.h"

CalendarsConfigDialog::CalendarsConfigDialog(QWidget *parent) :
    RotatingDialog(parent),
    ui(new Ui::CalendarsConfigDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->calendarList->setItemDelegate(new DetailedCalendarDelegate(ui->calendarList));

    QPushButton *newButton = new QPushButton(tr("New"));
    ui->buttonBox->addButton(newButton, QDialogButtonBox::ActionRole);
    connect(newButton, SIGNAL(clicked()), this, SLOT(newCalendar()));

    connect(ui->calendarList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onCalendarActivated(QListWidgetItem*)));
    connect(ui->calendarList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onCalendarChanged(QListWidgetItem*)));

    reload();

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

CalendarsConfigDialog::~CalendarsConfigDialog()
{
    delete ui;
}

void CalendarsConfigDialog::reload()
{
    ui->calendarList->clear();

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();
    CWrapper::sort(calendars);

    for (unsigned int c = 0; c < calendars.size(); c++) {
        using namespace Metrics::Item;

        QListWidgetItem *item = new QListWidgetItem();
        item->setData(IdRole, calendars[c]->getCalendarId());
        item->setData(NameRole, CWrapper::simplify(calendars[c]->getCalendarName(), TextMaxChars));
        item->setData(TypeRole, calendars[c]->getCalendarType());
        item->setData(ColorRole, calendars[c]->getCalendarColor());

        item->setCheckState(calendars[c]->IsShown() ? Qt::Checked : Qt::Unchecked);

        int error;
        item->setData(EventCountRole, mc->getEventCount(calendars[c]->getCalendarId(), error));
        item->setData(TodoCountRole, mc->getTodoCount(calendars[c]->getCalendarId(), error));
        item->setData(JournalCountRole, mc->getNoteCount(calendars[c]->getCalendarId(), error));

        ui->calendarList->addItem(item);
    }

    mc->releaseListCalendars(calendars);

    // Immediatedly adjust window size to the list of calendars
    ui->calendarList->updateGeometry();
    this->adjustSize();
}

void CalendarsConfigDialog::onCalendarActivated(QListWidgetItem *item)
{
    if ((new CalendarEditDialog(item->data(IdRole).toInt(), this))->exec() == QDialog::Accepted)
        reload();
}

void CalendarsConfigDialog::onCalendarChanged(QListWidgetItem *item)
{
    CMulticalendar *mc = CMulticalendar::MCInstance();
    int error;

    // Get the calendar representation
    CCalendar *calendar = mc->getCalendarById(item->data(IdRole).toInt(), error);

    // Abort on null item
    if (!calendar) return;

    // Modify the visibility
    calendar->setCalendarShown(item->checkState() == Qt::Checked);

    // Make changes persistent
    mc->modifyCalendar(calendar, error);

    delete calendar;

    ChangeManager::bump();

    reload();
}

void CalendarsConfigDialog::newCalendar()
{
    if ((new CalendarEditDialog(0, this))->exec() == QDialog::Accepted)
        reload();
    // TODO: Switch to ChangeClient? rembmer to sync!
}
