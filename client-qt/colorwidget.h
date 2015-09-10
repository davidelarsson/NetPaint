/*
 * colorwidget.h
 *
 * Skriven av David E Larsson
 *
 * 2012-12-12
 *
 * Representant för en färg i paletten.
 */
#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QWidget>

class ColorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWidget(QColor color);
    
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

signals:
    void colorInWidgetChanged(QColor color);

private:
    QColor color;
};

#endif // COLORWIDGET_H
