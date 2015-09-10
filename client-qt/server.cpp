#include "server.h"
#include <iostream>
#include <QtEndian>
#include <QBuffer>
using namespace std;

Server::Server(int port, int xSize, int ySize) :
    port(port)
{
    img = new QImage(xSize, ySize, QImage::Format_RGB888);
    img->fill(Qt::white);

    server = new QTcpServer();

    if(!server->listen(QHostAddress::Any, port)) {
        cout << "Could not open port: " << port << endl;
    } else {
        cout << "Listening on port: " << port << endl;
    }
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

Server::~Server() {
    server->disconnect();
    delete server;
    delete img;
}

void Server::stopListening() {
}

void Server::newConnection() {
    QTcpSocket *socket = server->nextPendingConnection();
    sockets.push_back(socket);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));

    cout << "img->byteCount(): " << img->byteCount() << endl;

    QImage imageWithAlpha(img->size(), QImage::Format_ARGB32);
    imageWithAlpha.fill(Qt::white);
    QPainter p(&imageWithAlpha);
    p.drawImage(0, 0, *img);
    p.end();

    sendImage(&imageWithAlpha, socket);
    emit statusChanged();
}

void Server::readyRead()
{
    cout << "Server::readyRead() called!" << endl;
    QTcpSocket *socket = (QTcpSocket *)sender();
    std::cout << "b" << std::endl;
//    if(socket->bytesAvailable() < 8) return;

    std::cout << "bytes available: " << socket->bytesAvailable() << std::endl;

    quint32 msg_size = qFromBigEndian(*(quint32 *)socket->peek(4).data());
    std::cout << "Size (first int read): " << msg_size << std::endl;

//    if(socket->bytesAvailable() < msg_size) return;

    socket->read(4);

    quint32 msg_type = qFromBigEndian(*(quint32 *)socket->read(4).data());
    std::cout << "Message type (second int read): " << msg_type << std::endl;

    if(msg_type == 2) {
        quint32 w = qFromBigEndian(*(quint32 *)socket->read(4).data());
        quint32 h = qFromBigEndian(*(quint32 *)socket->read(4).data());

        QByteArray imgData = qUncompress(socket->read(msg_size - 16));

        QImage imageMods((const uchar *)imgData.constData(), w, h, QImage::Format_ARGB32);

        QPainter paint(img);
        paint.setCompositionMode(QPainter::CompositionMode_SourceOver);
        paint.drawImage(QPoint(0, 0), imageMods);
        paint.end();

        for(int i = 0; i < sockets.size(); ++i) sendImage(&imageMods, sockets[i]);
        emit imageUpdated();
    }
    else if(msg_type == 4) {
    }
}

void Server::onDisconnect()
{
    QTcpSocket *socket = (QTcpSocket *)sender();
    for(int i = 0; i < sockets.size(); ++i) {
        if(sockets[i] == socket) {
            sockets.remove(i);
        }
    }
    emit statusChanged();
}

QString Server::getStatus()
{
    return QString("Number of clients connected: %1").arg(sockets.size());
}

void Server::sendImage(const QImage *image, QTcpSocket *socket) {
    cout << "Socket " << socket->socketDescriptor() << " sendImage();" << endl;
    uchar *ptr;
    QByteArray data;
    /* Start compression */
    data = qCompress(image->bits(), image->byteCount());
    /* End compression */

    /* Start uncompressed */
    //ptr = (uchar *) image->bits();
    //for(int i = 0; i < image->byteCount(); ++i) {
    //    data.append(ptr[i]);
    //}
    /* End uncompressed*/
    cout << "data.size(): " << data.size() << endl;

    quint32 msgSize = 16 + data.size();
    cout << "msgSize: " << msgSize << endl;

    quint32 to_write;
    to_write = qToBigEndian(msgSize);
    socket->write((char *)&to_write, 4);

    to_write = qToBigEndian(0x00000002);
    socket->write((char *)&to_write, 4);

    to_write = qToBigEndian(img->width());
    socket->write((char *)&to_write, 4);

    to_write = qToBigEndian(img->height());
    socket->write((char *)&to_write, 4);

    socket->write(data);
    socket->flush();

}

void Server::draw(const QImage *image)
{

    for(int i = 0; i < sockets.size(); ++i) {
        sendImage(image, sockets[i]);
    }
    QPainter painter(img);
    painter.drawImage(0, 0, *image);
    painter.end();
    emit imageUpdated();

}

const QImage *Server::getImage()
{
    return img;
}
