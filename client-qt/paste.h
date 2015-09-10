/*
 * paste.h
 *
 */

#ifndef PASTE_H
#define PASTE_H

#include "tool.h"
#include <QMouseEvent>

class Paste : public Tool
{
public:
    Paste(State *state);
    void clear() {}
    void mouseMove(QMouseEvent * ev) {}
    void mousePress(QMouseEvent * ev);
    void mouseRelease(QMouseEvent * ev) {}

private:
    State *state;
};

#endif // PASTE_H
