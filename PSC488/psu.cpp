#include "psu.hpp"

Psu::Psu(QTextBrowser *textBrowser) {
    this->port = new QSerialPort();
    this->textBrowser = textBrowser;

    QObject::connect(port, &QSerialPort::readyRead, this,
                     &Psu::serialReadyRead);

    serialResponse.clear();
}

Psu::~Psu() { delete port; }

void Psu::connect(const QString &com) {
    if (port->isOpen()) {
        textBrowser->insertPlainText(QString("Port is already open"));
        return;
    }

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
    // remote = query("REM") == "1";
}

bool Psu::powerSwitch() {
    // YES IT IS CORRECT (famous last words...)
    //
    // RSD OFF - disables RSD -> enables output
    // RSD ON  - enables RSD  -> disables output
    //
    // Example: SO:FU:RSD 1 - Will enable RSD, hence output will be disabled.
    // Example: SO:FU:RSD 0 - Will disable RSD, hence output will be enabled.

    while (true) {
        QString response = set("SO:FU:RSD", std::to_string(this->power));
        if (!response.endsWith("ER01")) {
            break;
        }
        QThread::msleep(10);
    }

    power = !power;
    return power;
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
    std::cout << "setting current" << std::endl;

    this->psuCurrent = current;
    set("SO:CU", current); // because of std::string() operator in fenced types
                           // we don't need to cast to string
}

void Psu::setVoltage(const Voltage voltage) {
    std::cout << "setting voltage" << std::endl;

    this->psuVoltage = voltage;
    set("SO:VO", voltage);
}

Current Psu::getPsuCurrentSettings() {
    return this->psuCurrent;
}
Voltage Psu::getPsuVoltageSettings() {
    return this->psuVoltage;
}

void Psu::measureMe() {
    try{
        measurePSUCurrent();
    }
    catch (CommException e) {
        std::cout << "caught" << std::endl;
    }
    try{
        measurePSUVoltage();
    }
    catch (CommException e) {
        std::cout << "caught" << std::endl;
    }
}

void Psu::setMeVoltage(Voltage voltage) {
    setVoltage(voltage);
    std::cout << "mVolt" << std::endl;
}
void Psu::setMeCurrent(Current current) {
    setCurrent(current);
    std::cout << "mCurr" << std::endl;
}

Current Psu::measurePSUCurrent() {
    while(true) {
        try {
            return _measurePSUCurrent();
        }
        catch (CommException e) {
            std::cout << "Failed conversion - trying again" << std::endl;
        }
    }
}

Voltage Psu::measurePSUVoltage() {
    while(true) {
        try {
            return _measurePSUVoltage();
        }
        catch (CommException e) {
            std::cout << "Failed conversion - trying again" << std::endl;
        }
    }
}

Current Psu::_measurePSUCurrent() {
    bool ok = false;
    auto response = query("ME:CU");

    double result = response.toDouble(&ok);
    if (ok) {
        this->measuredCurrent = Current(result);
        return measuredCurrent;
    }

    throw CommException("Current measurement conversion failed - output " + response.toStdString());
}

Voltage Psu::_measurePSUVoltage() {
    bool ok = false;
    auto response = query("ME:VO");

    double result = response.toDouble(&ok);
    if (ok) {
        this->measuredVoltage = Voltage(result);
        return measuredVoltage;
    }

    throw CommException("Voltage measurement conversion failed - output " + response.toStdString());
}

QString Psu::query(const std::string &query) {
    return SerialComm::send(this, this->port, query + "?", true);
}

QString Psu::set(const std::string &command, const std::string &arg) {
    auto _command = command;
    if (arg != "")
        _command += " " + arg;

    return SerialComm::send(this, this->port, _command, true);
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
