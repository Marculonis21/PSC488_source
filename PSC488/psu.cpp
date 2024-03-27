#include "psu.hpp"
#include <qnamespace.h>
#include <iostream>

Psu::Psu(QTextBrowser *textBrowser)
{
    this->port = new QSerialPort();
    this->textBrowser = textBrowser;

    QObject::connect(port, &QSerialPort::readyRead, 
                              this, 
                              &Psu::serialReadyRead);
}

Psu::~Psu()
{
    delete port;
}

void Psu::connect(const QString &com)
{
    port->setPortName(com);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);

    textBrowser->insertPlainText(QString("PSU (on %1) serial connected\n").arg(com));
    connected = true;

    port->clear();
    write("*IDN?");

    // CHECK IF it's powered up or not + Check if remote/local mode
    // power = ???
    // remote = ???
}

void Psu::powerSwitch()
{
    set("SO:FU:OUTP", std::to_string(!this->power));

    power = !power;
}

void Psu::remoteSwitch()
{
    if (this->remote) {
        set("LOC");
    }
    else {
        set("REM");
    }

    remote = !remote;
}

std::string Psu::query(const std::string &query)
{
    write(query+"?");

    // no return atm (create receive waiting methods)
    return "";
}

void Psu::set(const std::string &command, const std::string &arg)
{
    auto _command = command;
    if (arg != "") _command += " " + arg;

    write(_command);
}

void Psu::checkHealth()
{
    if(!isConnected()) {
        this->textBrowser->insertPlainText("PSU not connected");
        return;
    }

    std::cout << "check health" << std::endl;
    write("*TST?");
}

void Psu::write(const std::string &command)
{
    port->clear();
    port->setRequestToSend(false);
    port->write(command.data());
    port->flush();
    port->setRequestToSend(true);

    textBrowser->insertPlainText(QString("PSU command %1 sent\n").arg(command.data()));
}

void Psu::serialReadyRead()
{
    while(port->canReadLine()) {
        auto response = QString(port->readLine());
        textBrowser->insertPlainText(QString("received '%1'").arg(response));
    }
}

bool Psu::isConnected()
{
    return connected;
}

bool Psu::isTurnedOn()
{
    return power;
}

bool Psu::isRemote()
{
    return remote;
}
