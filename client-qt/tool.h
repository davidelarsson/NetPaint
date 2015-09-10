/*
 * tool.h
 *
 */
#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QMouseEvent>
#include <QColor>
#include "state.h"

class Tool:public QObject {
	State * state;
 public:
    virtual void setColor(QColor color) { this->color = color; }
    virtual void setState(State *state) { this->state = state; }
    virtual void clear() = 0;
    virtual void mouseMove(QMouseEvent * ev) = 0;
    virtual void mousePress(QMouseEvent * ev) = 0;
    virtual void mouseRelease(QMouseEvent * ev) = 0;
    QColor color;
};

#endif	// TOOL_H
