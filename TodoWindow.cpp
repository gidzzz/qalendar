#include "TodoWindow.h"

#include <QMaemo5Style>

#include <QDateTime>
#include <QShortcut>
#include <QTimer>

#include <CAlarm.h>

#include "CWrapper.h"

#include "ChangeManager.h"

TodoWindow::TodoWindow(CTodo *todo, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TodoWindow)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAttribute(Qt::WA_Maemo5StackedWindow);

    this->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
    this->setAttribute(Qt::WA_Maemo5LandscapeOrientation, false);
    this->setAttribute(Qt::WA_Maemo5PortraitOrientation, false);

    this->todo = CWrapper::details(todo);

    ui->doneBox->setContentsMargins(0,0,0,0);

    QPalette palette;
    palette.setColor(QPalette::WindowText, QMaemo5Style::standardColor("SecondaryTextColor"));
    ui->dueLabel->setPalette(palette);;
    ui->alarmLabel->setPalette(palette);
    ui->calendarInfo->setPalette(palette);
    ui->descriptionFrame->setPalette(palette);
    ui->descriptionInfo->setPalette(palette);

    connect(ui->editAction, SIGNAL(triggered()), this, SLOT(editTodo()));
    connect(ui->deleteAction, SIGNAL(triggered()), this, SLOT(deleteTodo()));

    connect(new QShortcut(QKeySequence(Qt::Key_Backspace), this), SIGNAL(activated()), this, SLOT(close()));

    reload();
}

TodoWindow::~TodoWindow()
{
    delete todo;
}

void TodoWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::ActivationChange
    &&  this->isActiveWindow()
    &&  this->isOutdated())
    {
        CTodo *outdatedTodo = todo;
        todo = CWrapper::details(todo);
        delete outdatedTodo;

        reload();
    }

    QMainWindow::changeEvent(e);
}

void TodoWindow::reload()
{
    // Gracefully recover from a task that was deleted behind the scenes
    if (!todo) {
        QTimer::singleShot(0, this, SLOT(close()));
        return;
    }

    this->sync();

    // Summary
    ui->summaryInfo->setText(QString::fromUtf8(todo->getSummary().c_str()));

    // Status
    ui->doneBox->setChecked(todo->getStatus());

    // Description
    QString description = QString::fromUtf8(todo->getDescription().c_str());
    ui->descriptionInfo->setText(description);
    ui->descriptionFrame->setHidden(description.isEmpty());

    // Deadline
    QDate due = QDateTime::fromTime_t(todo->getDue()).date();
    ui->dueInfo->setText(due.toString("dddd d MMMM yyyy"));

    // Deadline color
    QPalette palette;
    palette.setColor(QPalette::WindowText, !todo->getStatus() && due < QDate::currentDate()
                                           ? QMaemo5Style::standardColor("ActiveTextColor")
                                           : QMaemo5Style::standardColor("DefaultTextColor"));
    ui->dueInfo->setPalette(palette);

    // Deadline font
    QFont font = ui->dueInfo->font();
    font.setStrikeOut(todo->getStatus());
    ui->dueInfo->setFont(font);

    // Alarm
    CAlarm *alarm = todo->getAlarm();
    if (alarm) {
        ui->alarmLabel->show();
        ui->alarmInfo->show();
        ui->alarmInfo->setText(QDateTime::fromTime_t(alarm->getTrigger()).toString("dddd d MMMM yyyy, hh:mm"));
    } else {
        ui->alarmLabel->hide();
        ui->alarmInfo->hide();
    }

    // Extract calendar info
    int error;
    CCalendar *calendar = CMulticalendar::MCInstance()->getCalendarById(todo->getCalendarId(), error);
    QIcon calendarIcon = QIcon::fromTheme(CWrapper::colorIcon(calendar->getCalendarColor()));
    QString calendarName = QString::fromUtf8(calendar->getCalendarName().c_str());
    // TODO: Calendar type
    delete calendar;

    // Calendar info
    ui->calendarIcon->setPixmap(calendarIcon.pixmap(calendarIcon.availableSizes().first()));
    ui->calendarInfo->setText(calendarName);
}

void TodoWindow::editTodo()
{
    ChangeManager::edit(this, todo);
}

void TodoWindow::deleteTodo()
{
    ChangeManager::drop(this, todo);
}

void TodoWindow::closeEvent(QCloseEvent *e)
{
    if (todo && todo->getStatus() != ui->doneBox->isChecked()) {
        // Modify the status
        todo->setStatus(ui->doneBox->isChecked());

        // Alarm is not desired for finished tasks
        if (todo->getStatus())
            todo->removeAlarm();

        ChangeManager::save(todo);
    }

    QMainWindow::closeEvent(e);
}
