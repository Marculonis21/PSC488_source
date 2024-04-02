#include <QSerialPort>
#include <QCoreApplication>
#include <QThread>
#include <QTextEdit>
#include <QTextBrowser>
#include <QScrollBar>
#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>

#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

struct SerialComm
{
    template <typename T>
    static QString send(T *device, QSerialPort *port, const std::string &command, bool awaitResponse=true)
    {
        if (!port || !port->isOpen())
        {
            std::cout << "Send Error: Port not connected!" << std::endl;
            return "";
        }
        // clear wait response flag
        device->serialResponse.clear();

        port->clear(QSerialPort::AllDirections);

        port->setRequestToSend(false);

        port->write((command + "\r\n").data());
        port->flush();
        while(port->bytesToWrite())
        {
            QCoreApplication::processEvents(QEventLoop::ProcessEventsFlag::WaitForMoreEvents);
            QThread::msleep(10);
        }

        port->setRequestToSend(true);

        if (awaitResponse) return SerialComm::waitResponse(device);
        return "";
    }

    template<typename T>
    static void receive(T *device, QSerialPort *port, QTextBrowser *outputBrowser) {
        QString response;

        while(port->canReadLine()) {
            response = QString(port->readLine());
            device->serialResponse = response;
            std::cout << "response: " << response.toStdString() << std::endl;

            if(outputBrowser) {
                outputBrowser->insertPlainText(response);
                outputBrowser->verticalScrollBar()->setValue( outputBrowser->verticalScrollBar()->maximum() );
            }
        }
    }

    template<typename T>
    static QString waitResponse(T *device)
    {
        auto start = std::chrono::high_resolution_clock::now();

        while(device->serialResponse.isEmpty()) {
            QCoreApplication::processEvents(QEventLoop::ProcessEventsFlag::AllEvents);
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            if(duration.count() > 1000) {
                std::cout << "READ TIMEOUT - Device not connected/local error/no response/(high amount of steps)" << std::endl;
                break;
            }
        }

        return device->serialResponse;
    }
};

#endif // SERIAL_COMM_H
