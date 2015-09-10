/*
 * freehand.h
 */
#ifndef FREEHAND_H
#define FREEHAND_H

#include "tool.h"
#include <vector>

class FreeHand : public Tool
{
    int startX, startY;
    bool isDrawing;
    State *state;
    std::vector<int> mousePosX;
    std::vector<int> mousePosY;
public:
    FreeHand(State *state);
    void clear();
    void mouseMove(QMouseEvent * ev);
    void mousePress(QMouseEvent * ev);
    void mouseRelease(QMouseEvent * ev);
};
#endif // FREEHAND_H
