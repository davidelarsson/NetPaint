#ifndef SERVER_H
#define SERVER_H

#include "state.h"
#include <QImage>
#include <QPainter>
#include <QTcpServer>
#include <QTcpSocket>

class Server: public State {
    Q_OBJECT

public:
    Server(int port, int xSize, int ySize);
    ~Server();
    void draw(const QImage *image);
    const QImage *getImage();
    void stopListening();
    QString getStatus();
private slots:
    void newConnection();
    void readyRead();
    void onDisconnect();

private:
    void sendImage(const QImage *image, QTcpSocket *socket);

    QImage *img;
    uint port;
    QTcpServer *server;
    QVector<QTcpSocket *> sockets;
};

#endif				// SERVER_H
