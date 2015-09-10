/*
 * text.h
 */
#ifndef TEXT_H
#define TEXT_H

#include "tool.h"

class Text : public Tool
{
    int startX, startY;
    bool isDrawing;
    State *state;
    QString message;
public:
    Text(State *state);
    void clear();
    void mouseMove(QMouseEvent * ev);
    void mousePress(QMouseEvent * ev);
    void mouseRelease(QMouseEvent * ev);
};
#endif // TEXT_H
