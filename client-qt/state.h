/*
 * state.h
 *
 */

#ifndef STATE_H
#define STATE_H
#include <QImage>


class State : public QObject {
	Q_OBJECT
 public:
    virtual void draw(const QImage * what) = 0;

    virtual const QImage *getImage() = 0;
	
    virtual ~State() {};

    virtual QString getStatus() = 0;

 signals:
    void imageUpdated();
    void statusChanged();
};

#endif				// STATE_H
