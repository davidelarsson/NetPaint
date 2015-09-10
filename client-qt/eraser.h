/*
 * eraser.h
 *
 * Skriven av Ola Neselius
 *
 * 2012-12-12
 *
 * Sudd.
 */
#ifndef ERASER_H
#define ERASER_H
#include "tool.h"
#include <vector>

class Eraser : public Tool
{
    int startX, startY;
    bool isDrawing;
    State *state;
    std::vector<int> mousePosX;
    std::vector<int> mousePosY;
public:
    Eraser(State *state);
    void clear();
    void mouseMove(QMouseEvent * ev);
    void mousePress(QMouseEvent * ev);
    void mouseRelease(QMouseEvent * ev);
};

#endif // ERASER_H
