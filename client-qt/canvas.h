/*
 * canvas.h
 *
 */
#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include "tool.h"
#include "state.h"

class Canvas:public QWidget {
	Q_OBJECT

    Tool *activeTool;
    State *state;
 public:
    Canvas(QWidget *parent);

    void setActiveTool(Tool * tool) { activeTool = tool; }
    void setActiveState(State * state);
signals:
    void mouseMovedTo(QString string);

 protected:
	void paintEvent(QPaintEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
};

#endif				// CANVAS_H
