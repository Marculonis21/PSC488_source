#include "psu.hpp"

Psu::Psu(QTextBrowser *textBrowser)
{
    this->port = new QSerialPort();
    this->textBrowser = textBrowser;

    QObject::connect(port, &QSerialPort::readyRead, 
                              this, 
                              &Psu::serialReadyRead);

    serialResponse.clear();
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
    QThread::msleep(50);

    textBrowser->insertPlainText(query("*IDN"));

    // CHECK IF it's powered up or not + Check if remote/local mode
    power = query("SO:FU:OUTP") == "1";
    remote = query("REM") == "1";
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

QString Psu::query(const std::string &query)
{
    return SerialComm::send(this, this->port, query+"?");
}

void Psu::set(const std::string &command, const std::string &arg)
{
    auto _command = command;
    if (arg != "") _command += " " + arg;

    SerialComm::send(this, this->port, _command, false);
}

void Psu::checkHealth()
{
    if(!isConnected()) {
        this->textBrowser->insertPlainText("PSU not connected");
        return;
    }

    std::cout << "check health" << std::endl;
    textBrowser->insertPlainText(query("*TST"));
}

void Psu::serialReadyRead()
{
    SerialComm::receive(this, this->port, this->textBrowser);
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
