#include "ExpandingTextEdit.h"

#include <QScrollArea>
#include <QAbstractKineticScroller>

ExpandingTextEdit::ExpandingTextEdit(QWidget *parent) :
    QTextEdit(parent),
    inhibitAutoscroll(false),
    placeholderActive(false)
{
    connect(this, SIGNAL(textChanged()), this, SLOT(adjust()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(adjust()));

    adjust();
}

QString ExpandingTextEdit::toPlainText()
{
    // Ignore the placeholder if it is there
    return placeholderActive ? QString() : QTextEdit::toPlainText();
}

void ExpandingTextEdit::setPlainText(QString text)
{
    // Setting the text should not trigger scrolling of a parent
    inhibitAutoscroll = true;

    // Simulate a full focus cycle when setting the text
    // to not lose the track of placeholder status
    focusInEvent(NULL);
    QTextEdit::setPlainText(text);
    focusOutEvent(NULL);

    inhibitAutoscroll = false;
}

void ExpandingTextEdit::resizeEvent(QResizeEvent *e)
{
    // Change the width of the document to match the widget
    this->document()->setTextWidth(this->contentsRect().width());

    // Adjust height of the widget
    inhibitAutoscroll = true;
    adjust();
    inhibitAutoscroll = false;

    QTextEdit::resizeEvent(e);
}

void ExpandingTextEdit::showEvent(QShowEvent *e)
{
    // NOTE: If focus-out is called from the constructor, widget's palette
    // has wrong text color (e.g. white instead of black). To work around
    // this, the initial call to focus-out is postponed until a show event
    // occurs.
    focusOutEvent(NULL);

    QTextEdit::showEvent(e);
}

void ExpandingTextEdit::inputMethodEvent(QInputMethodEvent *e)
{
    // Simulate a focus-in on each VKB event
    focusInEvent(NULL);

    QTextEdit::inputMethodEvent(e);
}

void ExpandingTextEdit::focusInEvent(QFocusEvent *e)
{
    // Remove the placeholder if it is present
    if (placeholderActive) {
        // Placeholder will be gone, restore full opacity to the text
        QPalette palette = this->palette();
        QColor textColor = palette.color(QPalette::Text);
        textColor.setAlpha(255);
        palette.setColor(QPalette::Text, textColor);
        this->setPalette(palette);

        // Remove the placeholder
        this->clear();
        placeholderActive = false;
    }

    // Forward the event only if it was real
    if (e) QTextEdit::focusInEvent(e);
}

void ExpandingTextEdit::focusOutEvent(QFocusEvent *e)
{
    // Insert the placeholder if there is no other text to show
    if (this->document()->isEmpty()) {
        // Make the text half-tranparent to indicate that this is a placeholder
        QPalette palette = this->palette();
        QColor textColor = palette.color(QPalette::Text);
        textColor.setAlpha(127);
        palette.setColor(QPalette::Text, textColor);
        this->setPalette(palette);

        // Set the placeholder
        this->setPlainText(tr("Description"));
        placeholderActive = true;
    }

    // Forward the event only if it was real
    if (e) QTextEdit::focusOutEvent(e);
}

void ExpandingTextEdit::adjust()
{
    this->setMinimumHeight(qMax(105, (int) this->document()->size().height()
                                           + this->frameRect().height()
                                           - this->contentsRect().height()));

    if (inhibitAutoscroll) return;

    // Recursively go up the widget hierarchy to find a scroll area and ensure visibility of the cursor
    for (QWidget *parent = this; parent && parent->parentWidget(); parent = parent->parentWidget()) {
        if (QScrollArea *scrollArea = qobject_cast<QScrollArea*>(parent->parentWidget())) {
            const QRect cursor = this->cursorRect();
            scrollArea->property("kineticScroller").value<QAbstractKineticScroller*>()
                        ->ensureVisible(this->mapTo(scrollArea->widget(), cursor.center()),
                                        0, cursor.height() * 18/10);
            // NOTE: Every now and then the area is not scrolled by as much
            // as it should be. It's not much of an issue though, because
            // the next pressed key should trigger the scrolling again.
            break;
        }
    }
}
