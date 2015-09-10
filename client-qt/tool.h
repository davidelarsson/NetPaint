/*
 * tool.h
 *
 * Skriven av Roland Stenholm
 *
 * 2012-12-12
 *
 * Abstrakt klass för alla verktyg.
 */
#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QMouseEvent>
#include <QColor>
#include "state.h"

// Varje verkty implementerar denna klass.

class Tool:public QObject {
	// Varje verktyg måste känna till bilden så de vet hur stor den är, och
	// vissa verktyg måste kunna läsa i bilden (tex fyll).
	State * state;
 public:
    virtual void setColor(QColor color) { this->color = color; }
    virtual void setState(State *state) { this->state = state; }
    //virtual const QImage *getPreview() = 0;
    virtual void clear() = 0;
    virtual void mouseMove(QMouseEvent * ev) = 0;
    virtual void mousePress(QMouseEvent * ev) = 0;
    virtual void mouseRelease(QMouseEvent * ev) = 0;
    QColor color;
};

#endif	// TOOL_H
