/*
 * canvas.cpp
 *
 */

#include "canvas.h"
#include <QPainter>
#include <iostream>

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
}

void Canvas::setActiveState(State *s)
{
    this->state = s;
    connect(s, SIGNAL(imageUpdated()), this, SLOT(update()));
}


void Canvas::paintEvent(QPaintEvent *event)
{
    const QImage *img;
    img = state->getImage();
    QPainter paint(this);
    if(img) paint.drawImage(0, 0, *img);
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMovedTo(QString("X: %1, Y: %2").arg(event->x(), 4).arg(event->y(), 4));
    activeTool->mouseMove(event);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    activeTool->mousePress(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    activeTool->mouseRelease(event);
}
