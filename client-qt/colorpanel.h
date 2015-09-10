/*
 * colorpanel.h
 *
 * Skriven av David E Larsson
 *
 * 2012-12-12
 *
 * Färgpalett.
 */
#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <QWidget>
#include "colorwidget.h"


class ColorPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPanel(QWidget *parent = 0);
    
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    ~ColorPanel();

signals:
    void colorInPanelChanged(QColor color);

public slots:
    void colorWidgetSelected(QColor color);

private:
    QVector<ColorWidget *> colorWidgets;
    template <typename T, size_t N> size_t array_size(T(&) [N] ) { return N; }

};


#endif // COLORPANEL_H
