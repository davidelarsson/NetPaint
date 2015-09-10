/*
 * paste.cpp
 *
 * Skriven av Sofie Tapper och David E Larsson
 *
 * 2012-12-12
 *
 * Klistra in-verktyg.
 */
#include "paste.h"
#include <QClipboard>
#include <QPainter>
#include <QApplication>

Paste::Paste(State *state) :
    state(state)
{
}

void Paste::mousePress(QMouseEvent *ev)
{
    QImage image = QImage(state->getImage()->size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    QClipboard *clipboard = QApplication::clipboard();
    QImage clip = clipboard->image();
    painter.drawImage(ev->x(), ev->y(), clip);
    state->draw(&image);
}
