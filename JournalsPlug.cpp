#include "JournalsPlug.h"

#include <QMenu>
#include <QPushButton>
#include <QDateTime>
#include <QScrollBar>

#include <CMulticalendar.h>
#include <CJournal.h>

#include "JournalDelegate.h"

#include "CWrapper.h"

#include "ChangeManager.h"

JournalsPlug::JournalsPlug(QWidget *parent) :
    Plug(parent),
    ui(new Ui::JournalsPlug)
{
    ui->setupUi(this);

    ui->journalList->setItemDelegate(new JournalDelegate(ui->journalList));

    this->setAttribute(Qt::WA_Maemo5StackedWindow);

    // Create a button to add new notes
    QPushButton *newJournalButton = new QPushButton(QIcon::fromTheme("general_add"), tr("New note"));
    QListWidgetItem *item = new QListWidgetItem(ui->journalList);
    ui->journalList->setItemWidget(item, newJournalButton);

    connect(newJournalButton, SIGNAL(clicked()), this, SLOT(onJournalActivated()));
    connect(ui->journalList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onJournalActivated(QListWidgetItem*)));
    connect(ui->journalList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)));
}

JournalsPlug::~JournalsPlug()
{
    for (int i = 1; i < ui->journalList->count(); i++)
        delete qvariant_cast<CJournal*>(ui->journalList->item(i)->data(JournalRole));

    delete ui;
}

QString JournalsPlug::title() const
{
    QString title = tr("Notes");

    // The first item is always the button
    if (ui->journalList->count() > 1)
        title += " (" + QString::number(ui->journalList->count()-1) + ")";

    return title;
}

void JournalsPlug::onChange()
{
    reload();
}

void JournalsPlug::reload()
{
    this->sync();

    const int scrollPosition = ui->journalList->verticalScrollBar()->value();

    while (ui->journalList->count() > 1) {
        QListWidgetItem *item = ui->journalList->item(1);
        delete qvariant_cast<CJournal*>(item->data(JournalRole));
        delete item;
    }

    CMulticalendar *mc = CMulticalendar::MCInstance();
    vector<CCalendar*> calendars = mc->getListCalFromMc();
    vector<CJournal*> journals;
    map<int,int> palette;

    // Iterate over a list of calendars to get all journal items
    for (unsigned int i = 0; i < calendars.size(); i++) {
        if (!calendars[i]->IsShown()) continue;
        int error;
        vector<CJournal*> journalsPart = calendars[i]->getJournals(error);
        for (unsigned int j = 0; j < journalsPart.size(); j++)
            journalsPart[j]->setCalendarId(calendars[i]->getCalendarId()); // This is not done automatically

        journals.insert(journals.end(), journalsPart.begin(), journalsPart.end());

        palette[calendars[i]->getCalendarId()] = calendars[i]->getCalendarColor();
    }

    mc->releaseListCalendars(calendars);

    CWrapper::sort(journals);

    // Iterate over the list of journals and add them to the list widget
    for (unsigned int t = 0; t < journals.size(); t++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(JournalRole, QVariant::fromValue(journals[t]));
        item->setData(ColorRole, palette[journals[t]->getCalendarId()]);
        ui->journalList->addItem(item);
    }

    // Restore the original view
    ui->journalList->verticalScrollBar()->setValue(scrollPosition);

    emit titleChanged(title());
}

void JournalsPlug::onJournalActivated(QListWidgetItem *item)
{
    CJournal *journal = NULL;

    if (item)
        journal = qvariant_cast<CJournal*>(item->data(JournalRole));

    journal ? ChangeManager::edit(this, journal) : ChangeManager::newJournal(this);
}

void JournalsPlug::onContextMenuRequested(const QPoint &pos)
{
    if (ui->journalList->currentItem()->data(JournalRole).isNull()) return;

    QMenu contextMenu(this);
    contextMenu.addAction(tr("Edit"), this, SLOT(editCurrentJournal()));
    contextMenu.addAction(tr("Clone"), this, SLOT(cloneCurrentJournal()));
    contextMenu.addAction(tr("Delete"), this, SLOT(deleteCurrentJournal()));
    contextMenu.exec(this->mapToGlobal(pos));
}

void JournalsPlug::editCurrentJournal()
{
    ChangeManager::edit(this, qvariant_cast<CJournal*>(ui->journalList->currentItem()->data(JournalRole)));
}

void JournalsPlug::cloneCurrentJournal()
{
    ChangeManager::clone(this, qvariant_cast<CJournal*>(ui->journalList->currentItem()->data(JournalRole)));
}

void JournalsPlug::deleteCurrentJournal()
{
    ChangeManager::drop(this, qvariant_cast<CJournal*>(ui->journalList->currentItem()->data(JournalRole)));
}
