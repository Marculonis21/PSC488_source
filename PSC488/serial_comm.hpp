#include <QtSerialPort/QSerialPort>
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
        port->setRequestToSend(false);

        port->clear(QSerialPort::AllDirections);

        std::cout << command << std::endl;

        port->write((command + "\n").data());
        port->flush();
        std::cout << "sending" << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        while(port->bytesToWrite() && std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() < 100)
        {
            now = std::chrono::high_resolution_clock::now();
            QCoreApplication::processEvents(QEventLoop::ProcessEventsFlag::AllEvents);
            QThread::msleep(10);
            std::cout << "ss" << std::endl;
        }

        std::cout << "sent" << std::endl;

        port->setRequestToSend(true);

        if (awaitResponse) return SerialComm::waitResponse(device);
        return "";

    }

    template<typename T>
    static void receive(T *device, QSerialPort *port, QTextBrowser *outputBrowser) {
        QString response;

        while(port->canReadLine()) {
            response = QString(port->readLine());

            device->serialResponse = response.trimmed();
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
