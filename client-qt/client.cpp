/*
 * client.cpp
 *
 * Skriven av Roland Stenholm
 *
 * 2012-12-12
 *
 * Nätverksklient.
 */
#include "client.h"
#include <QtEndian>
#include "iostream"

using namespace std;

Client::Client(QHostAddress server_ip, int port)
{
    std::cout << server_ip.toString().toStdString() << std::endl;
	SrvConnection = new QTcpSocket(this);
	SrvConnection->connectToHost(server_ip, port);
    connect(SrvConnection, SIGNAL(readyRead()), this, SLOT(has_data()));
    img = new QImage();
    is_active = true;
    connect(SrvConnection, SIGNAL(disconnected()), this, SLOT(wasDisconnected()));
}

void Client::wasDisconnected()
{
    is_active = false;
    emit statusChanged();
}

QString Client::getStatus()
{
    return is_active ? "Active" : "Disconnected";
}

Client::~Client()
{
	SrvConnection->disconnectFromHost();
	delete SrvConnection;
	delete img;
}

void Client::draw(const QImage *what)
{
    QByteArray whatCompressed = qCompress(what->bits(), what->byteCount());
	quint32 msgSize = 16 + whatCompressed.size();
	
    quint32 to_write;

	to_write = qToBigEndian(msgSize);
    SrvConnection->write((char *)&to_write, 4);

    to_write = qToBigEndian(2); // Protocol number
    SrvConnection->write((char *)&to_write, 4);

	to_write = qToBigEndian(what->width());
    SrvConnection->write((char *)&to_write, 4);

	to_write = qToBigEndian(what->height());
    SrvConnection->write((char *)&to_write, 4);

	SrvConnection->write(whatCompressed);
    SrvConnection->flush();
}

/*
void Client::resize(uint w, uint h)
{
	QImage *resized_img = new QImage(w, h, QImage::Format_RGB32);
	
	QPainter paint(resized_img);
	paint.setCompositionMode(QPainter::CompositionMode_Source);
	paint.fillRect(0, 0, w, h, Qt::white);
	paint.setCompositionMode(QPainter::CompositionMode_Source);
	paint.drawImage(0, 0, *img);
	paint.end();
	
	delete img;
	img = resized_img;
}
*/

void Client::has_data()
{
    if(SrvConnection->bytesAvailable() < 8) return;

    std::cout << "bytes available: " << SrvConnection->bytesAvailable() << std::endl;

    quint32 msg_size = qFromBigEndian(*(quint32 *)SrvConnection->peek(4).data());
    std::cout << "Size (first int read): " << msg_size << std::endl;

    if(SrvConnection->bytesAvailable() < msg_size) return;

    SrvConnection->read(4);

	quint32 msg_type = qFromBigEndian(*(quint32 *)SrvConnection->read(4).data());
    std::cout << "Message type (second int read): " << msg_type << std::endl;

    if(msg_type == 2) {
        quint32 width = qFromBigEndian(*(quint32 *)SrvConnection->read(4).data());
        quint32 height = qFromBigEndian(*(quint32 *)SrvConnection->read(4).data());
        cout << "width: " << width << ", height: " << width << endl;

        QByteArray imgData = qUncompress(SrvConnection->read(msg_size - 16));
        std::cout << "imgData.size(): " << imgData.size() << std::endl;

        QImage *incomingCanvas = new QImage((const uchar *)imgData.constData(), width, height, QImage::Format_ARGB32);
        incomingCanvas->save("incomingFirst.jpg");
        QImage *newCanvas = new QImage(incomingCanvas->width(), incomingCanvas->height(), QImage::Format_ARGB32);
        QPainter painter(newCanvas);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(QPoint(0, 0), *incomingCanvas);
        delete incomingCanvas;
        newCanvas->save("incomingSecond.jpg");
        QImage *oldImage = img;
        img = newCanvas;
        delete oldImage;
        emit imageUpdated();
    }
/*
    if(msg_type == 2) {
		quint32 w = qFromBigEndian(*(quint32 *)SrvConnection->read(4).data());
		quint32 h = qFromBigEndian(*(quint32 *)SrvConnection->read(4).data());

        QByteArray imgData = qUncompress(SrvConnection->read(msg_size - 16));
        std::cout << "imgData.size(): " << imgData.size() << std::endl;
        QImage imageMods((const uchar *)imgData.constData(), w, h, QImage::Format_ARGB32);
		
        if(img->isNull() || w != img->width() || h != img->height()) resize(w, h);
		
        QPainter paint(img);
		paint.setCompositionMode(QPainter::CompositionMode_SourceOver);
        paint.drawImage(QPoint(0, 0), imageMods);
		paint.end();
		
		emit imageUpdated();
        std::cout << "Finished reading!" << std::endl;
	}
*/
	else if(msg_type == 4) {
	}
}

const QImage *Client::getImage()
{
    return img;
}
