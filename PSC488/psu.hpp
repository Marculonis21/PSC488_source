#include <QSerialPort>
#include <qobjectdefs.h>
#include <qwidget.h>
#include <qtextbrowser.h>
#include <string>
#include <memory>
#include "serial_comm.hpp"

#ifndef PSU_H
#define PSU_H

class Psu : public QObject
{
    Q_OBJECT;

public:
    Psu(QTextBrowser *textBrowser);
    virtual ~Psu();

    void set(const std::string &command, const std::string &arg="");
    QString query(const std::string &query);

    void connect(const QString &com);
    void powerSwitch();
    void remoteSwitch();

    void checkHealth();
    bool isConnected();
    bool isTurnedOn();
    bool isRemote();
    
    friend class SerialComm;

private:
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
