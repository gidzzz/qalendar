#include "CalendarEditDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMaemo5InformationBox>

#include <CalendarErrors.h>
#include <CMulticalendar.h>
#include <CCalendar.h>

#include "ColorPickSelector.h"

#include "ChangeManager.h"
#include "CWrapper.h"

CalendarEditDialog::CalendarEditDialog(int calendarId, QWidget *parent) :
    RotatingDialog(parent),
    ui(new Ui::CalendarEditDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    // Set up action buttons
    QPushButton *exportButton = new QPushButton(tr("Export"));
    QPushButton *deleteButton = new QPushButton(tr("Delete"));
    QPushButton *saveButton   = new QPushButton(tr("Save"));
    ui->buttonBox->addButton(exportButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(saveButton,   QDialogButtonBox::ActionRole);
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportCalendar()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteCalendar()));
    connect(saveButton,   SIGNAL(clicked()), this, SLOT(saveCalendar()));

    typeGroup = new QButtonGroup(this);
    typeGroup->addButton(ui->localTypeButton, LOCAL_CALENDAR);
    typeGroup->addButton(ui->birthdayTypeButton, BIRTHDAY_CALENDAR);
    ui->localTypeButton->setChecked(true);

    // Set up the color button
    ColorPickSelector *cps = new ColorPickSelector();
    ui->colorButton->setPickSelector(cps);

    if (calendarId) {
        // Edit calendar
        int error;
        calendar = CMulticalendar::MCInstance()->getCalendarById(calendarId, error);
        ui->typeInfo->setText(CWrapper::calendarType(calendar->getCalendarType()));
        ui->nameEdit->setText(QString::fromUtf8(calendar->getCalendarName().c_str()));
        ui->visibleBox->setChecked(calendar->IsShown());

        cps->setColor(calendar->getCalendarColor());

        ui->typeWidget->hide();
    } else {
        // New calendar
        calendar = new CCalendar();
        ui->visibleBox->setChecked(true);
        exportButton->setEnabled(false);
        deleteButton->setEnabled(false);

        // Count color occurences
        CMulticalendar *mc = CMulticalendar::MCInstance();
        vector<CCalendar*> calendars = mc->getListCalFromMc();
        vector<int> colorStats(COLOUR_NEXT_FREE);
        for (unsigned int i = 0; i < calendars.size(); i++)
            colorStats[calendars[i]->getCalendarColor()]++;
        mc->releaseListCalendars(calendars);

        // Find the least frequently used color
        int rarestColor = 0;
        for (unsigned int i = 1; i < colorStats.size(); i++)
            if (colorStats[i] < colorStats[rarestColor])
                rarestColor = i;

        cps->setColor(rarestColor);

        /*ui->typeInfo->hide();*/
        ui->typeWidget->hide();
    }

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

CalendarEditDialog::~CalendarEditDialog()
{
    delete calendar;

    delete ui;
}

// Create a new calendar or save changes to the existing one
void CalendarEditDialog::saveCalendar()
{
    // TODO: Make it possible to add the birthday calendar
    /*if (typeGroup->checkedId() == BIRTHDAY_CALENDAR)
        calendar->setCalendarType(BIRTHDAY_CALENDAR);*/

    calendar->setCalendarName(ui->nameEdit->text().toUtf8().data());
    calendar->setCalendarColor((CalendarColour) static_cast<ColorPickSelector*>(ui->colorButton->pickSelector())->currentColor());
    calendar->setCalendarShown(ui->visibleBox->isChecked());

    int error;

    if (calendar->getCalendarId() > 0) {
        CMulticalendar::MCInstance()->modifyCalendar(calendar, error);
    } else {
        CMulticalendar::MCInstance()->addCalendar(calendar, error);
    }

    ChangeManager::bump();

    if (error == CALENDAR_OPERATION_SUCCESSFUL) {
        this->accept();
    } else {
        QMaemo5InformationBox::information(this->parentWidget(), tr("Error occurred"));
    }
}

// Show file saving dialog and export the calendar
void CalendarEditDialog::exportCalendar()
{
    this->setEnabled(false);

    QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), "/home/user/MyDocs/.documents/File.ics");

    if (!filename.isEmpty()) {
        int error;
        calendar->exportCalendarEntries(filename.toUtf8().data(), error);
    }

    this->setEnabled(true);
}

// Delete the calendar
void CalendarEditDialog::deleteCalendar()
{
    if (QMessageBox::warning(this, " ", tr("Delete this calendar and its events, tasks and notes?"),
                             QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel) == QMessageBox::Yes)
    {
        int error;
        CMulticalendar::MCInstance()->deleteCalendar(calendar->getCalendarId(), error);

        ChangeManager::bump();

        this->accept();
    }
}
