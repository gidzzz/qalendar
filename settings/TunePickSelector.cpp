#include "TunePickSelector.h"

#include "TunePickDialog.h"

TunePickSelector::TunePickSelector(QString path, QObject *parent) :
    QMaemo5AbstractPickSelector(parent),
    path(path)
{
}

QString TunePickSelector::currentValueText() const
{
    return nameForPath(path);
}

QString TunePickSelector::currentPath() const
{
    return path;
}

QString TunePickSelector::nameForPath(QString path)
{
    // Recognize some default sounds
    if (path == "/usr/share/sounds/ui-calendar_alarm_default.aac")
        return "Chime";
    if (path == "/usr/share/sounds/ui-calendar_alarm_2.aac")
        return "Beta";
    if (path == "/usr/share/sounds/ui-calendar_alarm_3.aac")
        return "Gamma";
    if (path == "/usr/share/sounds/Beep.aac")
        return "Beep";

    // Extract the base name
    const int nameStart = path.lastIndexOf('/') + 1;
    const int nameEnd   = path.lastIndexOf('.');
    return path.mid(nameStart, nameEnd-nameStart);
}

QWidget* TunePickSelector::widget(QWidget *parent)
{
    TunePickDialog *dialog = new TunePickDialog(parent, path);

    connect(dialog, SIGNAL(selected(QString)), this, SLOT(onSelected(QString)));

    return dialog;
}

void TunePickSelector::onSelected(QString path)
{
    this->path = path;

    emit selected(currentValueText());
}
