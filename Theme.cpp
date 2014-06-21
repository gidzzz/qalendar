#include "Theme.h"

#include <QIcon>

#include "Metrics.h"

QPixmap* Theme::TodoEmblem;
QPixmap* Theme::BdayEmblem;

void Theme::init()
{
    using namespace Metrics::Pixmap;

    const QSize emblemSize(EmblemSize, EmblemSize);

    TodoEmblem = new QPixmap(QIcon::fromTheme("calendar_todo")
                             .pixmap(emblemSize).scaled(emblemSize));
    BdayEmblem = new QPixmap(QIcon::fromTheme("calendar_birthday")
                             .pixmap(emblemSize).scaled(emblemSize));
}
