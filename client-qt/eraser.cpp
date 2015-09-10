/*
 * eraser.cpp
 *
 * Skriven av Ola Neselius
 *
 * 2012-12-12
 *
 * Sudd.
 */
#include "eraser.h"
#include <QPainter>


Eraser::Eraser(State *state): state(state)
{
   clear();
}

void Eraser::clear()
{
    isDrawing = 0;
    mousePosX.clear();
    mousePosY.clear();
}

void Eraser::mousePress(QMouseEvent * ev)
{
    isDrawing = 1;
    startX = ev->x();
    startY = ev->y();
    mousePosX.push_back(startX);
    mousePosY.push_back(startY);
}

void Eraser::mouseMove(QMouseEvent * ev)
{
    if(isDrawing)
    {
        mousePosX.push_back(ev->x());
        mousePosY.push_back(ev->y());
    }
}

void Eraser::mouseRelease(QMouseEvent * ev)
{
    if(isDrawing)
    {
        QImage img(state->getImage()->size(), QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        QPen penna;
        penna.setWidth(10);
        penna.setColor(Qt::white);
        p.setPen(penna);

        for(unsigned int i = 0; i < mousePosX.size() - 1; i++)
        {
            p.drawLine(QLine(mousePosX[i], mousePosY[i], mousePosX[i+1], mousePosY[i+1]));
        }

        p.end();
        state->draw(&img);
        clear();
    }
}
