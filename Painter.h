#ifndef PAINTER_H
#define PAINTER_H

namespace Painter
{
    inline void drawStretched(QPainter &painter, const QPixmap &source, const QRect &dstRect)
    {
        QSize corner((source.width() - 1) / 2, (source.height() - 1) / 2);

        // Top left corner
        painter.drawPixmap(dstRect.x(), dstRect.y(), corner.width(), corner.height(),
                           source,
                           0, 0, corner.width(), corner.height());
        // Left edge
        painter.drawPixmap(dstRect.x(), dstRect.y()+corner.height(), corner.width(), dstRect.height()-corner.height()*2,
                           source,
                           0, corner.height(), corner.width(), 1);
        // Bottom left corner
        painter.drawPixmap(dstRect.x(), dstRect.y()+dstRect.height()-corner.height(), corner.width(), corner.height(),
                           source,
                           0, source.height()-corner.height(), corner.width(), corner.height());

        // Top edge
        painter.drawPixmap(dstRect.x()+corner.width(), dstRect.y(), dstRect.width()-corner.width()*2, corner.height(),
                           source,
                           corner.width(), 0, 1, corner.height());
        // Center
        painter.drawPixmap(dstRect.x()+corner.width(), dstRect.y()+corner.height(), dstRect.width()-corner.width()*2, dstRect.height()-corner.height()*2,
                           source,
                           corner.width(), corner.height(), 1, 1);
        // Bottom edge
        painter.drawPixmap(dstRect.x()+corner.width(), dstRect.y()+dstRect.height()-corner.height(), dstRect.width()-corner.width()*2, corner.height(),
                           source,
                           corner.width(), source.height()-corner.height(), 1, corner.height());

        // Top right corner
        painter.drawPixmap(dstRect.x()+dstRect.width()-corner.width(), dstRect.y(), corner.width(), corner.height(),
                           source,
                           source.width()-corner.width(), 0, corner.width(), corner.height());
        // Right edge
        painter.drawPixmap(dstRect.x()+dstRect.width()-corner.width(), dstRect.y()+corner.height(), corner.width(), dstRect.height()-corner.height()*2,
                           source,
                           source.width()-corner.width(), corner.height(), corner.width(), 1);
        // Bottom right corner
        painter.drawPixmap(dstRect.x()+dstRect.width()-corner.width(), dstRect.y()+dstRect.height()-corner.height(), corner.width(), corner.height(),
                           source,
                           source.width()-corner.width(), source.height()-corner.height(), corner.width(), corner.height());
    }
}

#endif // PAINTER_H
