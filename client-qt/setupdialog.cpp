/*
 * setupdialog.cpp
 *
 */
#include "setupdialog.h"
#include "ui_setupdialog.h"

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
}

SetupDialog::~SetupDialog()
{
    delete ui;
}

int SetupDialog::getPort()
{
    return ui->portSpinner->value();
}

int SetupDialog::xSize()
{
    return ui->xSize->value();
}

int SetupDialog::ySize()
{
    return ui->ySize->value();
}

QString SetupDialog::hostName()
{
    return ui->lineEdit->text();
}

bool SetupDialog::isServer() {
    return ui->radioButton->isChecked();
}

bool SetupDialog::newFile()
{
    return ui->radioButton_4->isChecked();
}
