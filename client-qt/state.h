/*
 * state.h
 *
 * Skriven av Roland Stenholm
 *
 * 2012-12-12
 *
 * Abstrakt klass för tillståndet, dvs vad vi tror att bilden är.
 */

#ifndef STATE_H
#define STATE_H
#include <QImage>

// Denna klass enkapsulerar bildens tillstånd. Vad som ska handa nar man forsoker rita och nar den
// uppdateras beror pa om vi ar server eller klient.

class State : public QObject {
	Q_OBJECT
 public:
    // Anropa denna metod med en QImage med samma storlek som bilden for att rita nat. Alfakanalen
    // maste sjalvklart vara 0 forutom dar det ska ritas.
    virtual void draw(const QImage * what) = 0;

    // Fa bilden
    virtual const QImage *getImage() = 0;
	
    virtual ~State() {};

    virtual QString getStatus() = 0;

 signals:
    // Avfyras da bilden har uppdaterats
    void imageUpdated();
    void statusChanged();
};

#endif				// STATE_H
