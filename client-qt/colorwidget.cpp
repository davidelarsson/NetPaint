/*
 * colorwidget.cpp
 *
 * Skriven av David E Larsson
 *
 * 2012-12-12
 *
 * Widget som visar färger.
 */
#include "colorwidget.h"
#include <QPainter>
#include <QRect>
#include <iostream>
#include <QMouseEvent>
#include <QColorDialog>

using namespace std;

ColorWidget::ColorWidget(QColor color) :
    color(color)
{
}

void ColorWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QRect rect(0, 0, this->width(), this->height());
    painter.fillRect(rect, color);
}

void ColorWidget::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::RightButton) {
        color = QColorDialog::getColor(color, NULL);
        emit colorInWidgetChanged(color);
        update();
        return;
    } else if(event->button() == Qt::LeftButton) {
        emit colorInWidgetChanged(color);
    }
}
