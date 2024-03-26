#include "psu.hpp"
#include <qnamespace.h>
#include <iostream>

Psu::Psu(QTextBrowser *textBrowser)
{
    this->mSerial = new QSerialPort();
    this->textBrowser = textBrowser;

    QObject::connect(mSerial, &QSerialPort::readyRead, 
                              this, 
                              &Psu::serialReadyRead);
}

Psu::~Psu()
{
    delete mSerial;
}

void Psu::connect(const QString &port)
{
    mSerial->setPortName(port);
    mSerial->setBaudRate(QSerialPort::Baud9600);
    mSerial->setDataBits(QSerialPort::Data8);
    mSerial->setStopBits(QSerialPort::OneStop);
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setFlowControl(QSerialPort::NoFlowControl);

    textBrowser->insertPlainText(QString("PSU (on %1) serial connected\n").arg(port));

    mSerial->clear();
    write("*IDN?");
}

void Psu::checkHealth()
{
    std::cout << "check health" << std::endl;
    write("*TST?");
}

void Psu::write(const std::string &command)
{
    mSerial->clear();
    mSerial->setRequestToSend(false);
    mSerial->write(command.data());
    mSerial->flush();
    mSerial->setRequestToSend(true);

    textBrowser->insertPlainText(QString("PSU command %1 sent\n").arg(command.data()));
}

void Psu::serialReadyRead()
{
    while(mSerial->canReadLine()) {
        auto response = QString(mSerial->readLine());
        textBrowser->insertPlainText(QString("received '%1'").arg(response));
    }
}
