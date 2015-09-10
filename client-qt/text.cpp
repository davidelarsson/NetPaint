/*
 * text.cpp
 *
 * Skriven av Sofie Tapper
 *
 * 2012-12-12
 *
 * Textverktyg.
 */
#include "text.h"
#include <QPainter>
#include <iostream>
#include <stdio.h>
#include <QInputDialog>

Text::Text(State *state): state(state)
{
   clear();
}

void Text::clear()
{
    isDrawing = 0;
}

void Text::mouseMove(QMouseEvent * ev)
{
}

void Text::mousePress(QMouseEvent * ev)
{
    isDrawing = 1;
    startX = ev->x();
    startY = ev->y();
    QImage img(state->getImage()->size(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter p(&img);
    QString text = QInputDialog::getText(NULL, tr("QInputDialog::getText()"),
                                            tr("Enter text:"), QLineEdit::Normal);

    p.setPen(color);
    p.drawText(startX, startY, text);
    message.clear();
    p.end();
    state->draw(&img);
    clear();
}

void Text::mouseRelease(QMouseEvent * ev)
{
}
