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

enum class FencedType { VOLTAGE, CURRENT, };

template <FencedType X> 
struct FencedValue {

    FencedValue() : value(0) {}
    explicit FencedValue(double voltage) : value(voltage) {}

    // get rid of those weird value copy assignments
    template <typename T> 
    FencedValue &operator=(const T &other) = delete;
    FencedValue &operator=(const FencedValue &other) {
        this->value = other.value;
        return *this;
    };

    bool operator<(const FencedValue &other) const { return value < other.value; }

    FencedValue operator-(const FencedValue &other) const { return FencedValue(value - other.value); }
    FencedValue operator+(const FencedValue &other) const { return FencedValue(value + other.value); }
    FencedValue operator-(double num) const { return FencedValue(value - num); }
    FencedValue operator+(double num) const { return FencedValue(value + num); }

    // conversion operator
    operator std::string() const { return std::to_string(value); }

  private:
    double value;
};

using Voltage = FencedValue<FencedType::VOLTAGE>;
using Current = FencedValue<FencedType::CURRENT>;

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
