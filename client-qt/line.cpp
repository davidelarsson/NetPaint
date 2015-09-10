/*
 */
#include "line.h"
#include <QPainter>

Line::Line(State *state): state(state)
{
   clear();
}

void Line::clear()
{
    isDrawing = 0;
}

void Line::mouseMove(QMouseEvent * ev)
{
}

void Line::mousePress(QMouseEvent * ev)
{
    isDrawing = 1;
    startX = ev->x();
    startY = ev->y();
}

void Line::mouseRelease(QMouseEvent * ev)
{
    if(isDrawing) {
        QImage img(state->getImage()->size(), QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setPen(color);
        p.drawLine(QLine(startX, startY, ev->x(), ev->y()));
        p.end();
        state->draw(&img);
        clear();
    }
}
