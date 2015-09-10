/*
 * client.h
 *
 * Skriven av Roland Stenholm
 *
 * 2012-12-12
 *
 * Detta är tillståndsabstraktionen då man är klient.
 */
#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QImage>
#include <QPainter>
#include "state.h"

class Client:public State {
	Q_OBJECT
	
    QImage *img;
	QTcpSocket *SrvConnection;
	void resize(uint w, uint h);
    bool is_active;
 private slots:
	void has_data();
    void wasDisconnected();
 public:
	Client(QHostAddress server_ip, int port);
	~Client();
	void draw(const QImage *);
    const QImage *getImage();
    QString getStatus();
};

#endif				// CLIENT_H
