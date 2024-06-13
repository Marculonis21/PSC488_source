#include "serial_comm.hpp"
#include <QSerialPort>
#include <exception>
#include <memory>
#include <mutex>
#include <qobjectdefs.h>
#include <qtextbrowser.h>
#include <qwidget.h>
#include <string>

#ifndef PSU_H
#define PSU_H

struct Voltage {
    Voltage() : value(0) {}
    Voltage(double voltage) : value(voltage) {}

    // get rid of those weird value assignments
    template <typename T> 
    Voltage& operator=(const T& other) = delete; 
    Voltage& operator=(const Voltage& other) {
        this->value = other.value;
        return *this;
    }; 


    /* Voltage(const Voltage &other) : value(other.value) {} */
    double operator()() const { return value; }
    bool operator<(const Voltage &other) const { return value < other.value; }

    Voltage operator-(const Voltage &other) const { return Voltage(value - other.value); }
    Voltage operator+(const Voltage &other) const { return Voltage(value + other.value); }
    Voltage operator-(double num) const { return Voltage(value - num); }
    Voltage operator+(double num) const { return Voltage(value + num); }

  private:
    double value;
};

struct Current {
    Current() : value(0) {};
    Current(double current) : value(current) {}
    Current& operator=(double& other) = delete; 

    template <typename T> 
    Current& operator=(const T& other) = delete; 
    Current& operator=(const Current& other) {
        this->value = other.value;
        return *this;
    }; 

    double operator()() const { return value; }
    bool operator<(const Current &other) const { return value < other.value; }

    Current operator-(const Current &other) const { return Current(value - other.value); }
    Current operator+(const Current &other) const { return Current(value + other.value); }
    Current operator-(double num) const { return Current(value - num); }
    Current operator+(double num) const { return Current(value + num); }

  private:
    double value;
};

class Psu : public QObject {
    Q_OBJECT;

  public:
    Psu(QTextBrowser *textBrowser);
    virtual ~Psu();

    void set(const std::string &command, const std::string &arg = "");
    QString query(const std::string &query);

    void connect(const QString &com);
    void powerSwitch();
    void remoteSwitch();

    void setCurrent(const Current current);
    void setVoltage(const Voltage voltage);

    Voltage measurePSUVoltage();
    Current measurePSUCurrent();

    Voltage getLastVoltage() const;
    Current getLastCurrent() const;

    void checkHealth();
    bool isConnected() const;
    bool isTurnedOn() const;
    bool isRemote() const;

    friend class SerialComm;

  private:
    std::mutex commLock;

    Voltage measuredVoltage;
    Current measuredCurrent;

    QSerialPort *port;
    QTextBrowser *textBrowser;
    bool connected = false;
    bool power = false;
    bool remote = false;
    QString serialResponse;

  private slots:
    void serialReadyRead();
};

#endif
