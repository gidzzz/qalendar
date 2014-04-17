#include "DateFormatWidget.h"

#include <QVBoxLayout>
#include <QMaemo5Style>

#include <QDate>

DateFormatWidget::DateFormatWidget(QWidget *parent)
{
    // Create subwidgets
    formatEdit = new QLineEdit(this);
    previewLabel = new QLabel(this);
    previewLabel->setIndent(8);

    // Adjust colors
    QPalette palette;
    palette.setColor(QPalette::WindowText, QMaemo5Style::standardColor("SecondaryTextColor"));
    previewLabel->setPalette(palette);

    // Adjust font
    QFont font = previewLabel->font();
    font.setPointSize(13);
    previewLabel->setFont(font);

    // Create the layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 5);
    layout->setSpacing(0);
    layout->addWidget(formatEdit);
    layout->addWidget(previewLabel);
}

void DateFormatWidget::setType(Date::Format type)
{
    this->type = type;

    // Load current format settings
    formatEdit->setText(Date::formatString(type));

    // Make the preview work
    onFormatChanged(formatEdit->text());
    connect(formatEdit, SIGNAL(textChanged(QString)), this, SLOT(onFormatChanged(QString)));
}

QString DateFormatWidget::format()
{
    return formatEdit->text();
}

void DateFormatWidget::onFormatChanged(QString format)
{
    previewLabel->setText(QLocale().toString(QDate::currentDate(), Date::sanitizeFormatString(format, type)));
}
