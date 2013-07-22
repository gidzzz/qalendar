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

    if (journal) journal = CWrapper::details(journal);

    CalendarPickSelector *cps = new CalendarPickSelector();
    ui->calendarButton->setPickSelector(cps);

    if (journal) {
        this->setWindowTitle("Edit note");

        // Load component data
        ui->summaryEdit->setPlainText(QString::fromUtf8(journal->getSummary().c_str()));
        cps->setCalendar(journal->getCalendarId());

        this->setupDeleteButton(ui->buttonBox, SLOT(deleteJournal()));
    } else {
        this->setWindowTitle("New note");

        journal = new CJournal();

        // Load last used settings
        QSettings settings;
        settings.beginGroup("JournalEditDialog");
        cps->setCalendar(settings.value("Calendar", 1).toInt());
    }

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

    QSettings settings;
    settings.beginGroup("JournalEditDialog");
    settings.setValue("Calendar", cps->currentId());

    this->accept();
}

void JournalEditDialog::deleteJournal()
{
    if (ChangeManager::drop(this, journal))
        this->accept();
}
