/*
 * ellips.h
 *
 */
#ifndef ELLIPS_H
#define ELLIPS_H

#include "tool.h"

class Ellips : public Tool
{
    int startX, startY;
    bool isDrawing;
    State *state;
public:
    Ellips(State *state);
    void clear();
    void mouseMove(QMouseEvent * ev);
    void mousePress(QMouseEvent * ev);
    void mouseRelease(QMouseEvent * ev);
};

#endif // ELLIPS_H
