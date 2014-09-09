#include "ComponentWidget.h"

#include <QPainter>
#include <QFontMetrics>
#include <QDateTime>
#include <QMenu>
#include <QMaemo5Style>

#include "CWrapper.h"

#include "Painter.h"
#include "Theme.h"
#include "Metrics.h"

#include "WeekWidget.h"

#include "EventWindow.h"
#include "TodoWindow.h"

#include "ChangeManager.h"

using namespace Metrics::ComponentWidget;
using namespace Metrics::Pixmap;

ComponentWidget::ComponentWidget(ComponentInstance *instance, int color, int x, int y, int w, int h, QWidget *parent) :
    QAbstractButton(parent),
    color(color),
    instance(instance),
    master(NULL)
{
    this->setGeometry(x, y, w, h);

    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
    connect(this, SIGNAL(pressed()), this, SLOT(onPressed()));
    connect(this, SIGNAL(released()), this, SLOT(onReleased()));
}

void ComponentWidget::addSlave(ComponentWidget *slave)
{
    slave->master = this;

    slaves.append(slave);
}

void ComponentWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    CComponent *component = instance->component;

    // Draw the background
    QPixmap background;
    if (this->isDown()) {
        background = QPixmap(PIX_GENERIC_PRESSED);
    } else {
        QIcon icon = QIcon::fromTheme(component->getAllDay() || component->getType() != E_EVENT
                                    ? CWrapper::colorAllDay(color)
                                    : CWrapper::colorHour(color));
        background = icon.pixmap(icon.availableSizes().first());
    }
    Painter::drawStretched(painter, background, this->rect());

    // Determine the available drawing area
    const int marginLeft   = 5;
    const int marginRight  = 4;
    const int marginTop    = 5;
    const int marginBottom = 4;
    QRect canvasRect(marginLeft, marginTop,
                     this->width()-marginLeft-marginRight,
                     this->height()-marginTop-marginBottom);

    QFont f = painter.font();
    painter.setPen(QMaemo5Style::standardColor("ReversedTextColor"));

    const bool alarm = component->getAlarm();
    const QString summary = CWrapper::summary(component, QDateTime::fromTime_t(instance->stamp).date());

    if (component->getAllDay() || component->getType() != E_EVENT) {
        // All-day event or not an event

        f.setPointSize(11);
        painter.setFont(f);

        if (component->getType() == E_EVENT) {
            // All-day event

            if (alarm)
                painter.drawPixmap(canvasRect.left(),
                                   canvasRect.top() + (canvasRect.height() - EmblemSize) / 2,
                                   QIcon::fromTheme("calendar_alarm").pixmap(EmblemSize, EmblemSize));
        } else {
            // Not an event

            // Decide which emblem to use
            QPixmap *emblem = NULL;
            switch (component->getType()) {
                case E_TODO: emblem = Theme::TodoEmblem; break;
                case E_BDAY: emblem = Theme::BdayEmblem; break;
            }

            if (alarm) {
                // Draw both the type emblem and the alarm emblem
                if (emblem)
                    painter.drawPixmap(canvasRect.left(),
                                       canvasRect.top(),
                                       *emblem);

                painter.drawPixmap(canvasRect.left(), canvasRect.bottom() - EmblemSize,
                                   QIcon::fromTheme("calendar_alarm").pixmap(EmblemSize, EmblemSize));
            } else {
                // Draw only the type emblem
                if (emblem)
                    painter.drawPixmap(canvasRect.left(),
                                       canvasRect.top() + (canvasRect.height() - EmblemSize) / 2,
                                       *emblem);
            }
        }

        // Update the available space after drawing the emblems
        if (component->getType() != E_EVENT || alarm)
            canvasRect.adjust(EmblemSize+4, 0, 0, 0);

        // Place the summary in the middle if it is smaller than the available space
        const QRect summaryRect = QFontMetrics(f).boundingRect(canvasRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, summary);
        if (summaryRect.height() < canvasRect.height())
            canvasRect.adjust(0, (canvasRect.height() - summaryRect.height()) / 2, 0, 0);

        // Draw the summary
        painter.drawText(canvasRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, summary);
    } else {
        // Standard event

        canvasRect.adjust(6, 0, 0, 0);

        f.setPointSize(10);
        painter.setFont(f);

        QFontMetrics fm(f);
        const int fontHeight = fm.height();
        const QRect summaryRect = fm.boundingRect(canvasRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, summary);

        const QString location = QString::fromUtf8(component->getLocation().c_str());
        const QString time = QDateTime::fromTime_t(instance->stamp).toString("hh:mm");

        const bool drawTime = !master && canvasRect.height() >= 2 * fontHeight;
        const bool drawLocation = canvasRect.height() - fontHeight - summaryRect.height() >= fontHeight;

        // Draw the alarm emblem if an alarm is present
        if (alarm && !master)
            painter.drawPixmap(canvasRect.right()-EmblemSize, canvasRect.top(),
                            QIcon::fromTheme("calendar_alarm").pixmap(EmblemSize, EmblemSize));

        // Draw the time if there is enough space
        if (drawTime) {
            // The approximate starting time can be told from WeekWidget's time axis,
            // so if the last two digits do not fit in the widget, it might be
            // pointless to draw the time. It might be better to leave the line
            // blank if there's an alarm so that the emblem is more visible.
            if (canvasRect.width() >= fm.width(time)) {
                painter.drawText(canvasRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, time);
                canvasRect.adjust(0, fontHeight, 0, 0);
            } else if (alarm) {
                canvasRect.adjust(0, fontHeight, 0, 0);
            }
        }

        // Draw summary
        painter.drawText(canvasRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, summary);
        canvasRect.adjust(0, summaryRect.height(), 0, 0);

        // Draw the location if there is enough space
        if (drawLocation) {
            painter.setPen(QMaemo5Style::standardColor("ReversedSecondaryTextColor"));
            painter.drawText(canvasRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, location);
        }
    }
}


void ComponentWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu contextMenu(this);
    contextMenu.addAction(tr("Edit"), this, SLOT(editComponent()));
    contextMenu.addAction(tr("Clone"), this, SLOT(cloneComponent()));
    contextMenu.addAction(tr("Delete"), this, SLOT(deleteComponent()));
    contextMenu.exec(e->globalPos());
}

void ComponentWidget::mousePressEvent(QMouseEvent *e)
{
    pressedPoint = e->pos();

    QAbstractButton::mousePressEvent(e);
}

void ComponentWidget::mouseReleaseEvent(QMouseEvent *e)
{
    pressedPoint = QPoint(); // Forget press

    QAbstractButton::mouseReleaseEvent(e);

    // If the widget was pressed and scrolling began, the release signal will
    // not be emitted, so treat mouse release event like a real trigger
    this->onReleased(true);
}

void ComponentWidget::mouseMoveEvent(QMouseEvent *e)
{
    using namespace Metrics::WeekWidget;

    if (pressedPoint.isNull()) return;

    // This discards a fake move event when scrolling starts
    if (e->pos() == QPoint(-1000,-1000)) return;

    if (WeekWidget *weekWidget = qobject_cast<WeekWidget*>(this->parentWidget())) {
        if (e->pos().x() - pressedPoint.x() > SwipeThold) {
            pressedPoint = QPoint(); // Forget press
            emit weekWidget->swipedPrev();
        } else if (e->pos().x() - pressedPoint.x() < -SwipeThold) {
            pressedPoint = QPoint(); // Forget press
            emit weekWidget->swipedNext();
        }
    }
}



void ComponentWidget::onClicked()
{
    switch (instance->component->getType()) {
        case E_EVENT:
        case E_BDAY:
            (new EventWindow(instance, this))->show();
            break;
        case E_TODO:
            (new TodoWindow(instance->todo, this))->show();
            break;
    }
}

void ComponentWidget::onPressed(bool real)
{
    if (real) {
        // Send a fake press to the master
        if (master != NULL)
            master->onPressed(false);
    } else {
        // Simulate a press
        this->setDown(true);
    }

    // Whether it was fake or not, all slaves should simulate a press
    for (int i = 0; i < slaves.size(); i++)
        slaves.at(i)->onPressed(false);
}

void ComponentWidget::onReleased(bool real)
{
    if (real) {
        // Send a fake release to the master
        if (master != NULL)
            master->onReleased(false);
    } else {
        // Simulate a release
        this->setDown(false);
    }

    // Whether it was fake or not, all slaves should be released
    for (int i = 0; i < slaves.size(); i++)
        slaves.at(i)->onReleased(false);
}

void ComponentWidget::editComponent()
{
    ChangeManager::edit(this, instance->component);
}

void ComponentWidget::cloneComponent()
{
    ChangeManager::clone(this, instance->component);
}

void ComponentWidget::deleteComponent()
{
    ChangeManager::drop(this, instance->component);
}
