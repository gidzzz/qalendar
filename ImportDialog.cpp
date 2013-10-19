#include "ImportDialog.h"

#include <QPushButton>
#include <QMaemo5InformationBox>

#include <QApplication>
#include <QTimer>
#include <QMaemo5Style>

#include <CMulticalendar.h>

#include "CalendarEditDialog.h"

#include "CWrapper.h"
#include "Roles.h"

const int batchSize = 100;

ImportDialog::ImportDialog(QWidget *parent, QString filename) :
    RotatingDialog(parent),
    ui(new Ui::ImportDialog),
    filename(filename),
    calendarId(0),
    eventCount(0),
    todoCount(0),
    journalCount(0),
    duplicates(false),
    aborted(false)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    // Adjust the color of the status text
    QPalette palette;
    palette.setColor(QPalette::WindowText, QMaemo5Style::standardColor("SecondaryTextColor"));
    ui->fileInfo->setPalette(palette);
    ui->contentInfo->setPalette(palette);

    // Display the file which is about to be imported
    ui->fileInfo->setText(filename.mid(filename.lastIndexOf('/')+1));

    // Set up the save button
    QPushButton *newButton = new QPushButton(tr("Import"));
    ui->buttonBox->addButton(newButton, QDialogButtonBox::ActionRole);
    connect(newButton, SIGNAL(clicked()), this, SLOT(importStart()));

    // Set up the option to create a new calendar
    QListWidgetItem *newItem = new QListWidgetItem();
    newItem->setData(NameRole, tr("New calendar"));
    newItem->setData(ColorRole, COLOUR_NEW);
    newItem->setData(VisibilityRole, true);
    ui->calendarList->insertItem(0, newItem);

    this->setFeatures(ui->dialogLayout, ui->buttonBox);

    // Prevent user input until loading is complete
    this->setEnabled(false);

    // Start loading file info
    QTimer::singleShot(0, this, SLOT(infoStart()));
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::closeEvent(QCloseEvent *e)
{
    // This should cancel both info and import
    CMulticalendar::MCInstance()->cancelImportIcsFileData();

    aborted = true;

    if (calendarId > 0)
        QMaemo5InformationBox::information(this->parentWidget(), tr("Importing aborted"));

    RotatingDialog::closeEvent(e);
}

void ImportDialog::infoStart()
{
    // Indicate busyness
    this->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
    QApplication::processEvents();

    infoStep();
}

void ImportDialog::infoStep()
{
    if (aborted) return;

    unsigned int eventCount;
    unsigned int todoCount;
    unsigned int journalCount;

    bool endOfFile = false;
    int error;

    // Analyze part of the file
    if (CMulticalendar::MCInstance()
        ->getICSFileInfoProgressive(filename.toUtf8().data(),
                                    eventCount, todoCount, journalCount,
                                    error, &endOfFile, batchSize))
    {
        this->eventCount   += eventCount;
        this->todoCount    += todoCount;
        this->journalCount += journalCount;

        ui->contentInfo->setText(tr("%n event(s)", "", this->eventCount)
                        + ", " + tr("%n task(s)" , "", this->todoCount)
                        + ", " + tr("%n note(s)" , "", this->journalCount));

        if (endOfFile) {
            // Loading complete
            this->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
            this->setEnabled(true);
        } else {
            // Another pass required
            QTimer::singleShot(0, this, SLOT(infoStep()));
        }
    } else {
        this->close();
        QMaemo5InformationBox::information(this->parentWidget(), tr("Error occurred"));
    }
}

void ImportDialog::importStart()
{
    QList<QListWidgetItem*> selection = ui->calendarList->selectedItems();
    if (!selection.isEmpty()) {
        calendarId = selection.first()->data(IdRole).toInt();

        // Create a new calendar if necessary
        if (!(calendarId > 0)) {
            CalendarEditDialog *ced = new CalendarEditDialog(0, this);

            // Propose a name for the new calendar
            const int nameStart = filename.lastIndexOf('/') + 1;
            const int nameEnd   = filename.lastIndexOf('.');
            ced->setCalendarName(filename.mid(nameStart, nameEnd-nameStart));

            if (ced->exec() == QDialog::Accepted) {
                calendarId = ced->calendarId();
            } else {
                return;
            }
        }

        // Indicate busyness
        this->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
        this->setEnabled(false);
        QApplication::processEvents();

        // Start importing
        importStep();
    }
}

void ImportDialog::importStep()
{
    if (aborted) return;

    string eventId;
    int eventDupes;
    int todoDupes;
    int journalDupes;

    bool endOfFile = false;
    int error;

    // Import part of the file
    if (CMulticalendar::MCInstance()
        ->importIcsFileDataProgressive(filename.toUtf8().data(), calendarId,
                                       eventId, todoDupes, eventDupes, journalDupes,
                                       error, &endOfFile, batchSize))
    {
        duplicates = duplicates || eventDupes || todoDupes || journalDupes;

        if (endOfFile) {
            // Importing complete
            calendarId = 0;
            this->close();
            if (duplicates)
                QMaemo5InformationBox::information(this->parentWidget(), tr("Some items already in calendar"));
        } else {
            // Another pass required
            QTimer::singleShot(0, this, SLOT(importStep()));
        }
    } else {
        this->close();
        QMaemo5InformationBox::information(this->parentWidget(), tr("Error occurred"));
    }
}
