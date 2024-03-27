#include <QSerialPort>
#include <qobjectdefs.h>
#include <qwidget.h>
#include <qtextbrowser.h>
#include <string>
#include <memory>

#ifndef PSU_H
#define PSU_H

class Psu : public QObject
{
    Q_OBJECT;

public:
    Psu(QTextBrowser *textBrowser);
    virtual ~Psu();
    void connect(const QString &com);
    void powerSwitch();
    void remoteSwitch();

    void set(const std::string &command, const std::string &arg="");
    std::string query(const std::string &query);

    void checkHealth();
    bool isConnected();
    bool isTurnedOn();
    bool isRemote();
    
private:
    QSerialPort *port;
    QTextBrowser *textBrowser;
    bool connected = false;
    bool power = false;
    bool remote = false;

    void write(const std::string &command);

private slots:
    void serialReadyRead();
};

#endif 
