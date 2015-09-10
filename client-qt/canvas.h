/*
 * canvas.h
 *
 * Skriven av Roland Stenholm och David E Larsson
 *
 * 2012-12-12
 *
 * Canvas-klassen är den widget som ritar upp själva bilden.
 */
#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include "tool.h"
#include "state.h"

// Denna klass är vår rityta. Dess ansvar är att rita upp bilden som fås av State och
// att skicka mushändelser till Tool.

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
	// Den som implementerar: Följande metoder anropas automatiskt då det händer nåt.
	// Anropa helt enkelt motsvarande metod i det aktiva verktyget.
	void paintEvent(QPaintEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
};

#endif				// CANVAS_H
