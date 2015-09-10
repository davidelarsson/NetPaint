/*
 * colorwidget.h
 *
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
