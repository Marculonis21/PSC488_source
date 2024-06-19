#include "threadWorker.hpp"
#include <string>
LiveMeasurementThread::LiveMeasurementThread(Plot *plot, Psu *psu,
                                             QLCDNumber *currLCD,
                                             QLCDNumber *voltLCD) {
    this->plot = plot;
    this->psu = psu;
    this->currLCD = currLCD;
    this->voltLCD = voltLCD;
    this->plot->clear();
    running = true;

    connect(this, &LiveMeasurementThread::plotRedraw, plot, &Plot::redraw);
    connect(this, &LiveMeasurementThread::measure, psu, &Psu::measureMe);
}

void LiveMeasurementThread::run() {
    Voltage measVO(0);
    Current measCU(0);

    int entryID = 0;

    while (running) {
        // try {
        //     measVO = this->psu->measurePSUVoltage();
        // }
        // catch (CommException e) {
        // }
        // try {
        //     measCU = this->psu->measurePSUCurrent();
        // }
        // catch (CommException e) {
        // }
        emit measure();
        measVO = psu->getLastVoltage();
        measCU = psu->getLastCurrent();
        // measCU = Current(QRandomGenerator::global()->bounded(0, 172));
        // measVO = Voltage(QRandomGenerator::global()->bounded(0, 5));

        currLCD->display(std::string(measCU).c_str());
        voltLCD->display(std::string(measVO).c_str());

        plot->appendData({(double)entryID}, {std::stod(std::string(measCU))});

        emit plotRedraw();
        entryID++;

        QThread::msleep(500);
    }

    emit endSignal();
}

void LiveMeasurementThread::stopMeasurement() {
    this->running = false;
}

PsuPowerThread::PsuPowerThread(Psu *psu, Voltage targetVoltage, Current targetCurrent) {
    this->psu = psu;

    connect(this, &PsuPowerThread::setVoltage, psu, Psu::setMeVoltage);
    connect(this, &PsuPowerThread::setCurrent, psu, Psu::setMeCurrent);

    this->targetVoltage = targetVoltage;
    this->targetCurrent = targetCurrent;
}

void PsuPowerThread::run() {
    const Voltage zero(0);

    Voltage measVO(0);
    Current measCU(0);

    setPsuTargets();

    while (running) {
        measVO = psu->getLastVoltage();
        measCU = psu->getLastCurrent();

        if (targetCurrent - 0.001 < measCU) {
            // psu->setVoltage(zero);
            emit setVoltage(zero);
            break;
        }
        // else {
        //     // psu->setVoltage(targetVoltage);
        //     emit setVoltage(targetVoltage);
        // }

        QThread::msleep(500);
    }

    // psu->setVoltage(Voltage(0));
    // psu->setCurrent(Current(0));

    emit endSignal();
}

void PsuPowerThread::changeTarget(Voltage voltage, Current current) {
    this->targetVoltage = voltage;
    this->targetCurrent = current;
    setPsuTargets();
}

void PsuPowerThread::setPsuTargets() {
    // psu->setVoltage(targetVoltage);
    // psu->setCurrent(targetCurrent);
    std::cout << "set voltage" << std::endl;
    emit setVoltage(this->targetVoltage);
    QThread::msleep(1000);

    std::cout << "set current" << std::endl;
    emit setCurrent(this->targetCurrent);
}

void PsuPowerThread::stopPower() {
    this->running = false;
}
