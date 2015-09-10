#include "fill.h"
#include <QPainter>
#include <QImage>
#include <iostream>

Fill::Fill(State *state): state(state)
{
   clear();
}

void Fill::clear()
{
    isDrawing = 0;
}

void Fill::mouseMove(QMouseEvent * ev)
{
    std::cout << "Mouse moved to: " << ev->x() << "," << ev->y() << std::endl;
}

void Fill::mousePress(QMouseEvent * ev)
{
    isDrawing = 1;
    std::cout << "Mouse button pressed in Fill at: " << ev->x() << "," << ev->y() << std::endl;
    startX = ev->x();
    startY = ev->y();
    QImage img(state->getImage()->size(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QColor targetColor = state->getImage()->pixel(startX, startY);
    if(targetColor == color) return;
    floodFill(startX, startY, targetColor.rgb(), color.rgb(), &img);
    state->draw(&img);
}

void Fill::mouseRelease(QMouseEvent * ev)
{
}

void Fill::floodFill(int x, int y, QRgb targetColor, QRgb replaceColor, QImage *img)
{
    if(x < 0 || y < 0) return;
    if(x >= state->getImage()->size().width() || y >= state->getImage()->size().height()) return;
    if(img->pixel(x, y) != Qt::transparent) return;
    if(state->getImage()->pixel(x, y) == targetColor)
    {
        img->setPixel(x, y, replaceColor);
        floodFill(x-1, y, targetColor, replaceColor, img);
        floodFill(x, y-1, targetColor, replaceColor, img);
        floodFill(x+1, y, targetColor, replaceColor, img);
        floodFill(x, y+1, targetColor, replaceColor, img);
    }
}
