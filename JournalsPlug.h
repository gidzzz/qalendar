#ifndef JOURNALSPLUG_H
#define JOURNALSPLUG_H

#include "Plug.h"

#include "ui_JournalsPlug.h"

class JournalsPlug : public Plug
{
    Q_OBJECT

public:
    JournalsPlug(QWidget *parent);
    ~JournalsPlug();

    QString title() const;
    bool isRotatable() const { return true; }

    void onChange();

private:
    Ui::JournalsPlug *ui;

    void reload();

private slots:
    void onJournalActivated(QListWidgetItem *item = 0);

    void onContextMenuRequested(const QPoint &pos);
    void editCurrentJournal();
    void cloneCurrentJournal();
    void deleteCurrentJournal();
};

#endif // JOURNALSPLUG_H
