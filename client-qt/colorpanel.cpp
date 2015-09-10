/*
 * colorpanel.cpp
 *
 * Skriven av David E Larsson
 *
 * 2012-12-12
 *
 * Färgpalett.
 */
#include "colorpanel.h"
#include <QPainter>
#include <QGridLayout>

using namespace std;

ColorPanel::ColorPanel(QWidget *parent):
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout();

    QVector<QColor> colorVector;
    colorVector.append(Qt::blue);
    colorVector.append(Qt::green);
    colorVector.append(Qt::black);
    colorVector.append(Qt::white);
    colorVector.append(Qt::yellow);
    colorVector.append(Qt::red);
    while(colorVector.size() < 20)
        colorVector.append(Qt::gray);

    for (int i = 0 ; i < colorVector.size() ; ++i) {
        ColorWidget *colorWidget = new ColorWidget(colorVector[i]);
        colorWidgets.push_back(colorWidget);
        layout->addWidget(colorWidgets[i], i % 2 , i/2);
        connect(colorWidget, SIGNAL(colorInWidgetChanged(QColor)), this, SLOT(colorWidgetSelected(QColor)));
    }
    this->setLayout(layout);
}

ColorPanel::~ColorPanel() {

    for (size_t i=0 ; i < colorWidgets.size() ; ++i) {
        delete colorWidgets[i];
    }

}

void ColorPanel::paintEvent(QPaintEvent *) {

}

void ColorPanel::mousePressEvent(QMouseEvent *) {
}

void ColorPanel::colorWidgetSelected(QColor color)
{
    emit colorInPanelChanged(color);
}
