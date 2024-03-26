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

    void connect(const QString &port);
    void checkHealth();
    bool isConnected();

private:
    QSerialPort *mSerial;
    QTextBrowser *textBrowser;
    bool connected = false;

    void write(const std::string &command);

private slots:
    void serialReadyRead();
};

#endif 
