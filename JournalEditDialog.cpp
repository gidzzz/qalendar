#include "JournalEditDialog.h"

#include <QDateTime>
#include <QSettings>

#include "CWrapper.h"

#include "CalendarPickSelector.h"

#include "ChangeManager.h"

JournalEditDialog::JournalEditDialog(QWidget *parent, CJournal *journal) :
    ComponentEditDialog(parent),
    ui(new Ui::JournalEditDialog)
{
    ui->setupUi(this);

    if (journal && !journal->getId().empty()) {
        journal = CWrapper::details(journal);

        this->setWindowTitle(tr("Edit note"));
        this->setupDeleteButton(ui->buttonBox, SLOT(deleteJournal()));
    } else {
        this->setWindowTitle(tr("New note"));
    }

    CalendarPickSelector *cps = new CalendarPickSelector();
    ui->calendarButton->setPickSelector(cps);

    if (journal) {
        // Do not set the default settings when editing an existing todo
        saveDefaults = false;

        // Load component data
        ui->summaryEdit->setPlainText(QString::fromUtf8(journal->getSummary().c_str()));
        cps->setCalendar(journal->getCalendarId());
    } else {
        journal = new CJournal();

        saveDefaults = true;

        // Load last used settings
        QSettings settings;
        settings.beginGroup("JournalEditDialog");
        cps->setCalendar(settings.value("Calendar", 1).toInt());
    }

    // An unsophisticated way to fill the available screen space
    this->setMinimumHeight(800);

    this->setupSaveButton(ui->buttonBox, SLOT(saveJournal()));

    this->setFeatures(ui->dialogLayout, ui->buttonBox);

    this->journal = journal;
}

JournalEditDialog::~JournalEditDialog()
{
    delete journal;

    delete ui;
}

void JournalEditDialog::saveJournal()
{
    // Do not allow empty journal entries
    if (ui->summaryEdit->toPlainText().isEmpty()) {
        ui->summaryEdit->setFocus();
        return;
    }

    CalendarPickSelector *cps = qobject_cast<CalendarPickSelector*>(ui->calendarButton->pickSelector());

    journal->setSummary(ui->summaryEdit->toPlainText().toUtf8().data());
    journal->setDateStart(QDateTime::currentDateTime().toTime_t());

    ChangeManager::save(journal, cps->currentId());

    if (saveDefaults) {
        QSettings settings;
        settings.beginGroup("JournalEditDialog");
        settings.setValue("Calendar", cps->currentId());
    }

    this->accept();
}

void JournalEditDialog::deleteJournal()
{
    if (ChangeManager::drop(this, journal))
        this->accept();
}
