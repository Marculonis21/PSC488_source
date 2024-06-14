#include "psu.hpp"
#include <mutex>
#include <string>

Psu::Psu(QTextBrowser *textBrowser) {
    this->port = new QSerialPort();
    this->textBrowser = textBrowser;

    QObject::connect(port, &QSerialPort::readyRead, this,
                     &Psu::serialReadyRead);

    serialResponse.clear();
}

Psu::~Psu() { delete port; }

void Psu::connect(const QString &com) {
    port->setPortName(com);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::TwoStop);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);

    if (port->open(QIODevice::ReadWrite)) {
        textBrowser->insertPlainText(
            QString("PSU (on %1) serial start\n").arg(com));
    } else {
        textBrowser->insertPlainText("Serial port not connected!\n");
        return;
    }

    connected = true;

    port->clear();
    QThread::msleep(50);

    query("*IDN");

    // CHECK IF it's powered up or not + Check if remote/local mode
    power = query("SO:FU:RSD") == "0";
    remote = query("REM") == "1";
}

void Psu::powerSwitch() {
    // Example: SO:FU:RSD 1 - Will enable RSD, hence output will be disabled.
    // Example: SO:FU:RSD 0 - Will disable RSD, hence output will be enabled.

    set("SO:FU:RSD", std::to_string(this->power));
    power = !power;
}

void Psu::remoteSwitch() {
    if (this->remote) {
        set("LOC");
    } else {
        set("REM");
    }

    remote = !remote;
}

void Psu::setCurrent(const Current current) {
    std::lock_guard guard(commLock);

    // because of std::string() operator we don't need to cast to string
    set("SO:CU", current); //
}

void Psu::setVoltage(const Voltage voltage) {
    std::lock_guard guard(commLock);

    set("SO:VO", voltage);
}

Current Psu::measurePSUCurrent() {
    std::lock_guard guard(commLock);

    bool ok = false;
    auto response = query("ME:CU");

    double result = response.toDouble(&ok);
    if (ok) {
        this->measuredCurrent = Current(result);
        return result;
    }

    textBrowser->insertPlainText(
        QString("Current measurement conversion failed - output %1\n")
            .arg(response));
    return -1;
}

Voltage Psu::measurePSUVoltage() {
    std::lock_guard guard(commLock);

    bool ok = false;
    auto response = query("ME:VO");

    double result = response.toDouble(&ok);
    if (ok) {
        this->measuredVoltage = Voltage(result);
        return result;
    }

    textBrowser->insertPlainText(
        QString("Voltage measurement conversion failed - output %1\n")
            .arg(response));
    return -1;
}

QString Psu::query(const std::string &query) {
    return SerialComm::send(this, this->port, query + "?");
}

void Psu::set(const std::string &command, const std::string &arg) {
    auto _command = command;
    if (arg != "")
        _command += " " + arg;

    SerialComm::send(this, this->port, _command, false);
}

void Psu::checkHealth() {
    if (!isConnected()) {
        this->textBrowser->insertPlainText("PSU not connected");
        return;
    }

    std::cout << "check health" << std::endl;
    textBrowser->insertPlainText(query("*TST"));
}

void Psu::serialReadyRead() {
    SerialComm::receive(this, this->port, this->textBrowser);
}

Voltage Psu::getLastVoltage() const { return this->measuredVoltage; }
Current Psu::getLastCurrent() const { return this->measuredCurrent; }

bool Psu::isConnected() const { return connected; }
bool Psu::isTurnedOn() const { return power; }
bool Psu::isRemote() const { return remote; }
