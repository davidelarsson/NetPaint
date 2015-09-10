/*
 * ellips.cpp
 *
 * Skriven av Ola Neselius
 *
 * 2012-12-12
 *
 * Verktyg som ritar en ellips.
 */
#include "ellips.h"
#include <QPainter>

Ellips::Ellips(State *state): state(state)
{
   clear();
}

void Ellips::clear()
{
    isDrawing = 0;
}

void Ellips::mouseMove(QMouseEvent * ev)
{
}

void Ellips::mousePress(QMouseEvent * ev)
{
    isDrawing = 1;
    startX = ev->x();
    startY = ev->y();
}

void Ellips::mouseRelease(QMouseEvent * ev)
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
                p.drawEllipse(ev->x(), ev->y(), startX - ev->x(), startY - ev->y());
            }
            else
            {
                p.drawEllipse(ev->x(), startY, startX - ev->x(), ev->y() - startY);
            }
        }
        else
        {
            if(startY > ev->y())
            {
                p.drawEllipse(startX, ev->y(), ev->x() - startX, startY - ev->y());
            }
            else
            {
                p.drawEllipse(startX, startY, ev->x() - startX, ev->y() - startY);
            }
        }

        p.end();
        state->draw(&img);
        clear();
    }
}
