#ifndef FILL_H
#define FILL_H

#include "tool.h"

class Fill : public Tool
{
    int startX, startY;
    bool isDrawing;
    State *state;
public:
    Fill(State *state);
    void clear();
    void floodFill(int x, int y, QRgb targetColor, QRgb replaceColor, QImage *img);
    void mouseMove(QMouseEvent * ev);
    void mousePress(QMouseEvent * ev);
    void mouseRelease(QMouseEvent * ev);
};

#endif // FILL_H
