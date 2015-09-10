/*
 * mainwindow.cpp
 *
 * 2012-12-12
 *
 * Kod för huvudfönstret.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "setupdialog.h"
#include "client.h"
#include <QtGui>
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget * parent):
    QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

    SetupDialog dialog;
    dialog.setModal(true);
    if(dialog.exec() != dialog.Accepted)
        exit(0);
    int port = dialog.getPort();
    if(dialog.isServer() && dialog.newFile()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                            "Save Image",                                 // Caption
                                                            "",                                           // Default selection
                                                             "Images (*.png *.jpg);;XML files (*.xml);;All files (*)"); // Filter
            QImage image;
            image.load(fileName);
            state = new Server(port, image.width(), image.height());
            state->draw(&image);
    } else if(dialog.isServer()) {
        int xSize = dialog.xSize();
        int ySize = dialog.ySize();
        state = new Server(port, xSize, ySize);
    } else {
        QHostAddress host = QHostAddress(dialog.hostName());
        cout << "We're a client with the host address: ";
        cout << dialog.hostName().toStdString() << endl;
        state = new Client(host, port);
    }

    ui->statusBar->showMessage(state->getStatus());
    line = new Line(state);
    rect = new Rect(state);
    freeHand = new FreeHand(state);
    text = new Text(state);
    ellips = new Ellips(state);
    fill = new Fill(state);
    erasor = new Eraser(state);
    paste = new Paste(state);

    ui->canvas->setActiveState(state);
    ui->canvas->setActiveTool(freeHand);
    text->setState(state);

    connect(ui->colorPanel, SIGNAL(colorInPanelChanged(QColor)), this, SLOT(colorChanged(QColor)));
//    connect(ui->canvas, SIGNAL(mouseMovedTo(QString)), ui->statusBar, SLOT(showMessage(QString)));
    connect(state, SIGNAL(statusChanged()), this, SLOT(updateStatus()));
    ui->canvas->setMinimumSize(QSize(state->getImage()->width(), state->getImage()->height()));
}

MainWindow::~MainWindow()
{
    delete erasor;
    delete fill;
    delete ellips;
    delete text;
    delete freeHand;
    delete rect;
    delete line;
    delete state;
	delete ui;
}

void MainWindow::on_toolButtonFreeHand_clicked()
{
    ui->canvas->setActiveTool(freeHand);
}

void MainWindow::on_toolButtonLine_clicked()
{
        ui->canvas->setActiveTool(line);
}

void MainWindow::on_toolButtonRectangle_clicked()
{
        ui->canvas->setActiveTool(rect);
}

void MainWindow::on_toolButtonEllips_clicked()
{
        ui->canvas->setActiveTool(ellips);
}

void MainWindow::on_toolButtonText_clicked()
{
        ui->canvas->setActiveTool(text);
}

void MainWindow::on_toolButtonFill_clicked()
{
        ui->canvas->setActiveTool(fill);
}

void MainWindow::on_actionNew_triggered()
{
    SetupDialog dialog;
    dialog.setModal(true);
    if(dialog.exec() != dialog.Accepted)
        return;
    int port = dialog.getPort();
    int xSize = dialog.xSize();
    int ySize = dialog.ySize();
    Server *tmpState = new Server(port, xSize, ySize);

    line = new Line(tmpState);
    rect = new Rect(tmpState);
    freeHand = new FreeHand(tmpState);
    text = new Text(tmpState);
    ellips = new Ellips(tmpState);
    fill = new Fill(tmpState);
    erasor = new Eraser(tmpState);

    ui->canvas->setActiveState(tmpState);
    ui->canvas->setActiveTool(fill);
    text->setState(tmpState);
    delete state;
    state = tmpState;
}

void MainWindow::on_action_Save_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
              "Save Image",                                 // Caption
              "",                                           // Default selection
               "Images (*.png *.jpg);;XML files (*.xml);;All files (*)"); // Filter
    if(!state->getImage()->save(fileName))
        QMessageBox::critical(this, "ERROR!", "Could not save file!");
}

void MainWindow::on_toolButtonErasor_clicked()
{
    ui->canvas->setActiveTool(erasor);
}

void MainWindow::colorChanged(QColor color)
{
    line->setColor(color);
    freeHand->setColor(color);
    rect->setColor(color);
    text->setColor(color);
    ellips->setColor(color);
}

void MainWindow::on_toolButton_clicked()
{
    ui->canvas->setActiveTool(paste);
}

void MainWindow::updateStatus()
{
    ui->statusBar->showMessage(state->getStatus());
}

void MainWindow::on_actionAbout_NetPaint_triggered()
{
    QMessageBox::about(this, "About NetPaint",
                        "A paint program with networking support.");

}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}
