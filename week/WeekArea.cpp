#include "WeekArea.h"

#include <QScrollBar>
#include <QEvent>

WeekArea::WeekArea(QWidget *parent) :
    QScrollArea(parent)
{
    WeekWidget *weekWidget = new WeekWidget();

    this->setWidget(weekWidget);

    topHintWidget = new WeekHintWidget(WeekHintWidget::Top, weekWidget, this);
    bottomHintWidget = new WeekHintWidget(WeekHintWidget::Bottom, weekWidget, this);

    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onScrolled(int)));
}

// Return the child widget, cast it for convenience
WeekWidget* WeekArea::widget()
{
    return qobject_cast<WeekWidget*>(QScrollArea::widget());
}

// Handle resize events to make sure that hint widgets have the desired geometry
bool WeekArea::viewportEvent(QEvent *e)
{
    if (e->type() == QEvent::Resize) {
        // Stick the top widget to top, bottom widget to bottom, adjust widths
        topHintWidget->setGeometry(0, 0, this->viewport()->width(), 4);
        bottomHintWidget->setGeometry(0, this->viewport()->height()-4, this->viewport()->width(), 4);

        // Position of the bottom hint widget might have changed, refresh it
        onScrolled(this->verticalScrollBar()->value());
    }

    return QScrollArea::viewportEvent(e);
}

// Handle scrolling by forwarding the new position to hint widgets
void WeekArea::onScrolled(int position)
{
    topHintWidget->setPosition(position);
    bottomHintWidget->setPosition(position + viewport()->height()-1);
}
