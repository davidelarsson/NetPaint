#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetupDialog(QWidget *parent = 0);
    ~SetupDialog();
    int getPort();
    int xSize();
    int ySize();
    QString hostName();
    
    bool isServer();
    bool newFile();

private:
    Ui::SetupDialog *ui;
    int port;
};

#endif // SETUPDIALOG_H
