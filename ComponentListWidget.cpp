#include "ComponentListWidget.h"

#include <QMenu>
#include <QScrollBar>

#include "ComponentListDelegate.h"
#include "EventWindow.h"
#include "TodoWindow.h"

#include "ChangeManager.h"
#include "ComponentInstance.h"
#include "CWrapper.h"

#include "Roles.h"
#include "Metrics.h"

ComponentListWidget::ComponentListWidget(QWidget *parent) :
    QListWidget(parent)
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setItemDelegate(new ComponentListDelegate(this));

    headingWidget = new DateHeadingWidget(this);
    headingWidget->hide();

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)));
    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onComponentActivated(QListWidgetItem*)));
}

void ComponentListWidget::setFloatingHeadings(bool enable)
{
    // NOTE: There is no protection from multiple connections.
    enable ? connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onScrolled(int))) :
          disconnect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onScrolled(int)));
}

void ComponentListWidget::setDate(QDate date)
{
    this->date = date;
}

bool ComponentListWidget::viewportEvent(QEvent *e)
{
    if (e->type() == QEvent::Resize)
        headingWidget->setGeometry(0, 0, this->viewport()->width(), 30);

    return QListWidget::viewportEvent(e);
}

void ComponentListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QListWidget::mouseReleaseEvent(e);

    this->clearSelection();
}

void ComponentListWidget::onContextMenuRequested(const QPoint &pos)
{
    // NOTE: If this class ever causes a segfault, that might be because either
    // there is no currrent item (seemingly impossible without keyboard
    // shortcuts) or component on the current item is null (impossible without
    // changing the current item between invoking the menu and executing one of
    // its actions?).

    // Ignore the button and day headings
    if (this->currentItem()->data(ComponentRole).isNull()) return;

    QMenu *contextMenu = new QMenu(this);
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);;
    contextMenu->addAction(tr("Edit"), this, SLOT(editCurrentComponent()));
    contextMenu->addAction(tr("Delete"), this, SLOT(deleteCurrentComponent()));
    contextMenu->exec(this->mapToGlobal(pos));
}

void ComponentListWidget::onComponentActivated(QListWidgetItem *item)
{
    // Ignore day headings
    if (item->data(HeadingRole).toBool()) return;

    // Check if the activated item holds a component
    if (ComponentInstance *instance = qvariant_cast<ComponentInstance*>(item->data(ComponentRole))) {
        // Perform an action which depends on the type of the component
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
}

void ComponentListWidget::onScrolled(int position)
{
    using namespace Metrics::Item;

    if (position == 0) {
        headingWidget->hide();
    } else {
        QListWidgetItem *firstBelow = this->itemAt(0, HeadingHeight);
        headingWidget->setDate(this->itemAt(0, 0)->data(DateRole).toDate());
        headingWidget->move(0, firstBelow->data(HeadingRole).isNull() ? 0 : this->visualItemRect(firstBelow).top() - HeadingHeight);
        headingWidget->show();
    }
}

void ComponentListWidget::newEvent()
{
    ChangeManager::newEvent(this, QDateTime(date, QTime::currentTime()));
}

void ComponentListWidget::newTodo()
{
    ChangeManager::newTodo(this, date);
}

void ComponentListWidget::editCurrentComponent()
{
    CComponent *component = qvariant_cast<ComponentInstance*>(this->currentItem()->data(ComponentRole))->component;

    ChangeManager::edit(this, component);
}

void ComponentListWidget::deleteCurrentComponent()
{
    CComponent *component = qvariant_cast<ComponentInstance*>(this->currentItem()->data(ComponentRole))->component;

    ChangeManager::drop(this, component);
}
