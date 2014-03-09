#include "CalendarEditDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMaemo5InformationBox>
#include <QButtonGroup>

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

    // Set up the color button
    ColorPickSelector *cps = new ColorPickSelector();
    ui->colorButton->setPickSelector(cps);

    CMulticalendar *mc = CMulticalendar::MCInstance();

    if (calendarId) {
        // Load an existing calendar
        int error;
        calendar = mc->getCalendarById(calendarId, error);
        ui->typeInfo->setText(CWrapper::calendarType(calendar->getCalendarType()));
        ui->nameEdit->setText(QString::fromUtf8(calendar->getCalendarName().c_str()));
        ui->visibleBox->setChecked(calendar->IsShown());

        cps->setColor(calendar->getCalendarColor());

        // Set up action buttons
        QPushButton *exportButton = new QPushButton(tr("Export"));
        QPushButton *deleteButton = new QPushButton(tr("Delete"));
        ui->buttonBox->addButton(exportButton, QDialogButtonBox::ActionRole);
        ui->buttonBox->addButton(deleteButton, QDialogButtonBox::ActionRole);
        connect(exportButton, SIGNAL(clicked()), this, SLOT(exportCalendar()));
        connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteCalendar()));

        // Do not allow calendar type editing
        ui->typeWidget->hide();

        this->setWindowTitle(tr("Edit calendar"));
    } else {
        // Create a new calendar
        calendar = new CCalendar();
        ui->visibleBox->setChecked(true);

        // Count color occurences
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

        // Try not to allow multiple birthday calendars
        if (CCalendar *birthdayCalendar = mc->getBirthdayCalendar()) {
            // Disable the birthday calendar option
            delete birthdayCalendar;
            ui->typeWidget->hide();
        } else {
            // Enable the birthday calendar option
            QButtonGroup *typeGroup = new QButtonGroup(this);
            typeGroup->addButton(ui->localTypeButton, LOCAL_CALENDAR);
            typeGroup->addButton(ui->birthdayTypeButton, BIRTHDAY_CALENDAR);
            ui->localTypeButton->setChecked(true);

            connect(typeGroup, SIGNAL(buttonClicked(int)), this, SLOT(onTypeButtonClicked(int)));
        }

        ui->typeInfo->hide();

        this->setWindowTitle(tr("New calendar"));
    }

    // Set up the save button
    QPushButton *saveButton = new QPushButton(tr("Save"));
    ui->buttonBox->addButton(saveButton, QDialogButtonBox::ActionRole);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveCalendar()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);
}

CalendarEditDialog::~CalendarEditDialog()
{
    delete calendar;

    delete ui;
}

int CalendarEditDialog::calendarId()
{
    return calendar->getCalendarId();
}

void CalendarEditDialog::setCalendarName(QString name)
{
    ui->nameEdit->setText(name);
}

void CalendarEditDialog::onTypeButtonClicked(int type)
{
    switch (type) {
        case LOCAL_CALENDAR:
            if (ui->nameEdit->text() == "cal_ti_smart_birthdays")
                ui->nameEdit->setText(QString());
            break;
        case BIRTHDAY_CALENDAR:
            if (ui->nameEdit->text().isEmpty())
                ui->nameEdit->setText("cal_ti_smart_birthdays");
            break;
    }
}

// Create a new calendar or save changes to the existing one
void CalendarEditDialog::saveCalendar()
{
    CMulticalendar *mc = CMulticalendar::MCInstance();

    if (ui->birthdayTypeButton->isChecked()) {
        // Do not allow multiple birthday calendars
        if (CCalendar *birthdayCalendar = mc->getBirthdayCalendar()) {
            delete birthdayCalendar;
            QMaemo5InformationBox::information(this->parentWidget(), tr("Error occurred"));
            return;
        }
        calendar->setCalendarType(BIRTHDAY_CALENDAR);
    } else {
        calendar->setCalendarType(LOCAL_CALENDAR);
    }

    calendar->setCalendarName(ui->nameEdit->text().toUtf8().data());
    calendar->setCalendarColor((CalendarColour) static_cast<ColorPickSelector*>(ui->colorButton->pickSelector())->currentColor());
    calendar->setCalendarShown(ui->visibleBox->isChecked());

    int error;

    if (calendar->getCalendarId() > 0) {
        mc->modifyCalendar(calendar, error);
    } else {
        mc->addCalendar(calendar, error);
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
