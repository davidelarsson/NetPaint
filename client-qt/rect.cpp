/*
 * rect.cpp
 *
 * Skriven av Sofie Tapper
 *
 * 2012-12-12
 *
 * Rektangelverktyg.
 */
#include "rect.h"
#include <QPainter>

Rect::Rect(State *state): state(state)
{
   clear();
}

void Rect::clear()
{
    isDrawing = 0;
}

void Rect::mouseMove(QMouseEvent * ev)
{
}

void Rect::mousePress(QMouseEvent * ev)
{
    isDrawing = 1;
    startX = ev->x();
    startY = ev->y();
}

void Rect::mouseRelease(QMouseEvent * ev)
{
    if(isDrawing) {
        QImage img(state->getImage()->size(), QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setPen(color);
        if(startX > ev->x())
        {
            if(startY > ev->y())
            {
                p.drawRect(ev->x(), ev->y(), startX - ev->x(), startY - ev->y());
            }
            else
            {
                p.drawRect(ev->x(), startY, startX - ev->x(), ev->y() - startY);
            }
        }
        else
        {
            if(startY > ev->y())
            {
                p.drawRect(startX, ev->y(), ev->x() - startX, startY - ev->y());
            }
            else
            {
                p.drawRect(startX, startY, ev->x() - startX, ev->y() - startY);
            }
        }

        p.end();
        state->draw(&img);
        clear();
    }
}

