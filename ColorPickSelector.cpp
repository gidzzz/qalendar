#include "ColorPickSelector.h"

#include <QMaemo5ValueButton>

#include "CWrapper.h"

#include "ColorPickDialog.h"

ColorPickSelector::ColorPickSelector(QObject *parent) : QMaemo5AbstractPickSelector(parent)
{
    color = -1;
}

void ColorPickSelector::setColor(int color)
{
    this->color = color;

    // Set the icon in the parent button
    QMaemo5ValueButton *button = qobject_cast<QMaemo5ValueButton*>(this->parent());
    if (button)
        button->setIcon(QIcon::fromTheme(CWrapper::colorIcon(color)));
}

QString ColorPickSelector::currentValueText() const
{
    // The value text for this button type is supposed to be empty
    return QString();
}

int ColorPickSelector::currentColor() const
{
    return color;
}

QWidget* ColorPickSelector::widget(QWidget *parent)
{
    ColorPickDialog *dialog = new ColorPickDialog(color, parent);

    connect(dialog, SIGNAL(selected(int)), this, SLOT(onSelected(int)));

    return dialog;
}

void ColorPickSelector::onSelected(int color)
{
    setColor(color);
}
