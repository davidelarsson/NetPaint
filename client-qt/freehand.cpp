/*
 * freehand.cpp
 *
 * Skriven av Sofie Tapper
 *
 * 2012-12-12
 *
 * Frihandsritverktyg.
 */
#include "freehand.h"
#include <QPainter>

FreeHand::FreeHand(State *state): state(state)
{
   clear();
}

void FreeHand::clear()
{
    isDrawing = 0;
    mousePosX.clear();
    mousePosY.clear();
}

void FreeHand::mouseMove(QMouseEvent * ev)
{
    if(isDrawing)
    {
        mousePosX.push_back(ev->x());
        mousePosY.push_back(ev->y());
    }
}

void FreeHand::mousePress(QMouseEvent * ev)
{
    isDrawing = 1;
    startX = ev->x();
    startY = ev->y();
    mousePosX.push_back(startX);
    mousePosY.push_back(startY);
}

void FreeHand::mouseRelease(QMouseEvent * ev)
{
    if(isDrawing)
    {
        QImage img(state->getImage()->size(), QImage::Format_ARGB32);
        img.fill(Qt::transparent);
        QPainter p(&img);
        p.setPen(color);

        for(unsigned int i = 0; i < mousePosX.size() - 1; i++)
        {
            p.drawLine(QLine(mousePosX[i], mousePosY[i], mousePosX[i+1], mousePosY[i+1]));
        }

        p.end();
        state->draw(&img);
        clear();
    }
}
