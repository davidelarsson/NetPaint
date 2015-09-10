/*
 * rect.h
 *
 */
#ifndef RECT_H
#define RECT_H

#include "tool.h"

class Rect : public Tool
{
    int startX, startY;
    bool isDrawing;
    State *state;
public:
    Rect(State *state);
    void clear();
    void mouseMove(QMouseEvent * ev);
    void mousePress(QMouseEvent * ev);
    void mouseRelease(QMouseEvent * ev);
};

#endif // RECT_H
