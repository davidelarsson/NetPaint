/*
 * text.h
 *
 * Skriven av Sofie Tapper
 *
 * 2012-12-12
 *
 * Verktyg för att rita text.
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
