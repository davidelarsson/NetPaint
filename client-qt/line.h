/*
 */
#ifndef LINE_H
#define LINE_H

#include "tool.h"

class Line : public Tool
{
    int startX, startY;
    bool isDrawing;
    State *state;
public:
    Line(State *state);
	void clear();
    void mouseMove(QMouseEvent * ev);
    void mousePress(QMouseEvent * ev);
    void mouseRelease(QMouseEvent * ev);
};

#endif // LINE_H
