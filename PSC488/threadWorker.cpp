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
}

void LiveMeasurementThread::run() {
    Voltage measVO;
    Current measCU;

    int entryID = 0;

    while (running) {
        /* measVO = this->psu->measurePSUVoltage(); */
        /* measCU = this->psu->measurePSUCurrent(); */
        measCU = Current(QRandomGenerator::global()->bounded(0, 172));
        measVO = Voltage(QRandomGenerator::global()->bounded(0, 5));

        currLCD->display(std::string(measCU).c_str());
        voltLCD->display(std::string(measVO).c_str());

        plot->appendData({(double)entryID}, {std::stod(std::string(measCU))});

        emit plotRedraw();
        entryID++;

        QThread::msleep(25);
    }

    emit endSignal();
}

void LiveMeasurementThread::stopPower() {
    this->running = false;
}

PsuPowerThread::PsuPowerThread(Psu *psu) {
    this->psu = psu;
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
            psu->setVoltage(zero);
        }
        else {
            psu->setVoltage(targetVoltage);
        }

        QThread::msleep(500);
    }

    psu->setVoltage(Voltage(0));
    psu->setCurrent(Current(0));

    emit endSignal();
}

void PsuPowerThread::changeTarget(Voltage voltage, Current current) {
    this->targetVoltage = voltage;
    this->targetCurrent = current;
    setPsuTargets();
}

void PsuPowerThread::setPsuTargets() {
    psu->setVoltage(targetVoltage);
    psu->setCurrent(targetCurrent);
}

void PsuPowerThread::stopPower() {
    this->running = false;
}
