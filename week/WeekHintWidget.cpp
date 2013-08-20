#include "WeekHintWidget.h"

#include <QPainter>

#include "CWrapper.h"

WeekHintWidget::WeekHintWidget(Placement placement, WeekWidget *weekWidget, QWidget *parent) :
    QWidget(parent),
    weekWidget(weekWidget),
    placement(placement)
{
}

void WeekHintWidget::paintEvent(QPaintEvent *)
{
    using namespace Metrics::WeekWidget;

    QPainter painter(this);

    const WeekHintProfile &profile = weekWidget->hintProfile;

    if (placement == Top) {
        for (unsigned int f = 0; f < profile.upView.size(); f++) {
            if (profile.upView[f].y <= position) {
                for (unsigned int h = 0; h < profile.upView[f].hints.size(); h++) {
                    WeekHintProfile::Hint hint = profile.upView[f].hints[h];

                    QIcon stripe = QIcon::fromTheme(CWrapper::colorStripe(hint.color));
                    painter.drawPixmap(hint.left, 0, hint.right-hint.left+1, this->height(),
                                       stripe.pixmap(stripe.availableSizes().first()),
                                       3, 3, 1, 1);
                }
                break;
            }
        }
    } else

    if (placement == Bottom) {
        for (unsigned int f = 0; f < profile.dnView.size(); f++) {
            if (profile.dnView[f].y >= position) {
                for (unsigned int h = 0; h < profile.dnView[f].hints.size(); h++) {
                    WeekHintProfile::Hint hint = profile.dnView[f].hints[h];

                    QIcon stripe = QIcon::fromTheme(CWrapper::colorStripe(hint.color));
                    painter.drawPixmap(hint.left, 0, hint.right-hint.left+1, this->height(),
                                       stripe.pixmap(stripe.availableSizes().first()),
                                       3, 3, 1, 1);
                }
                break;
            }
        }
    }
}

void WeekHintWidget::setPosition(int position)
{
    this->position = position;

    this->update();
}

