#include "fencedType.hpp"
#include "serial_comm.hpp"
#include <QSerialPort>
#include <exception>
#include <memory>
#include <qobjectdefs.h>
#include <qtextbrowser.h>
#include <qwidget.h>
#include <string>

#ifndef PSU_H
#define PSU_H

class CommException : public std::exception {
  public:
    CommException(std::string msg) : message(msg) {}
    const char *what() const throw() { return message.c_str(); }

  private:
    std::string message;
};

class Psu : public QObject {
    Q_OBJECT;

  public:
    Psu(QTextBrowser *textBrowser);
    virtual ~Psu();

    void set(const std::string &command, const std::string &arg = "");
    QString query(const std::string &query);

    void connect(const QString &com);
    bool powerSwitch();
    void remoteSwitch();

    void setCurrent(const Current current);
    void setVoltage(const Voltage voltage);
    Current getPsuCurrentSettings();
    Voltage getPsuVoltageSettings();

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
    Voltage measuredVoltage;
    Current measuredCurrent;

    Voltage psuVoltage;
    Current psuCurrent;

    QSerialPort *port;
    QTextBrowser *textBrowser;
    bool connected = false;
    bool power = false;
    bool remote = false;
    QString serialResponse;

  private slots:
    void serialReadyRead();

  public slots:
    void measureMe();
    void setMeVoltage(Voltage voltage);
    void setMeCurrent(Current current);
};

#endif
