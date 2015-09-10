/*
 * mainwindow.h
 *
 * Skriven av David E Larsson, Roland Stenholm, Sofie Tapper
 *
 * 2012-12-12
 *
 * Själva fönstret.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "line.h"
#include "rect.h"
#include "tool.h"
#include "freehand.h"
#include "text.h"
#include "ellips.h"
#include "eraser.h"
#include "fill.h"
#include "paste.h"
#include <QMouseEvent>

namespace Ui {
	class MainWindow;
}
class MainWindow:public QMainWindow {
	Q_OBJECT
 public:
	 explicit MainWindow(QWidget * parent = 0);
	~MainWindow();

    State *state;
private slots:
    void on_toolButtonFreeHand_clicked();
    void on_toolButtonLine_clicked();
    void on_toolButtonRectangle_clicked();
    void on_toolButtonEllips_clicked();
    void on_toolButtonText_clicked();
    void on_toolButtonFill_clicked();
    void on_actionNew_triggered();
    void on_action_Save_triggered();
    void on_toolButtonErasor_clicked();
    void colorChanged(QColor color);
    void on_toolButton_clicked();
    void updateStatus();
    void on_actionAbout_NetPaint_triggered();
    void on_actionAbout_Qt_triggered();

private:
	 Ui::MainWindow * ui;
     Tool *line;
     Tool *rect;
     Tool *freeHand;
     Tool *text;
     Tool *ellips;
     Tool *fill;
     Tool *erasor;
     Tool *paste;
};

#endif				// MAINWINDOW_H
