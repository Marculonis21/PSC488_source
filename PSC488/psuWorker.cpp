#include "psuWorker.hpp"
#include <qtimer.h>

PsuWorker::PsuWorker(Psu *psu, Plot *plot, QLCDNumber *currLCD, QLCDNumber *voltLCD) {
    this->psu = psu;
    this->plot = plot;
    this->plot->clear();

    this->currLCD = currLCD;
    this->voltLCD = voltLCD;

    this->zero = Voltage(0);
    
    connect(this, &PsuWorker::measure, this, &PsuWorker::runMeasurement); 
    connect(this, &PsuWorker::plotRedraw, plot, &Plot::redraw);
}

void PsuWorker::stop() {
    this->running = false;
}

void PsuWorker::run() {
    this->running = true;
    QTimer::singleShot(250, this, &PsuWorker::measure);
}

void PsuWorker::runMeasurement() {
    if (!running) {
        psu->setVoltage(zero);
        return;
    }

    measVO = psu->measurePSUVoltage();
    measCU = psu->measurePSUCurrent();

    // if (targetCurrent - 0.001 < measCU && psu->getPsuVoltageSettings() != zero) {
    //     psu->setVoltage(zero);
    // }
    if (psu->getPsuVoltageSettings() != targetVoltage){
        psu->setVoltage(targetVoltage);
    }

    runDataCollection();
}

void PsuWorker::runDataCollection() {
    currLCD->display(std::string(measCU).c_str());
    voltLCD->display(std::string(measVO).c_str());

    plot->appendData({(double)plotEntryID}, {std::stod(std::string(measCU))});
    plotEntryID++;
    emit plotRedraw();

    QTimer::singleShot(500, this, &PsuWorker::measure);
}

// Values offsetting measured instrumental difference
const double voltageMultiplier = 1/1.9;
const double currentMultiplier = 1/5.3;
void PsuWorker::setTargets(Voltage tVoltage, Current tCurrent) {
    std::cout << "worker Targets set" << std::endl;
    this->targetVoltage = tVoltage*voltageMultiplier;
    this->targetCurrent = tCurrent*currentMultiplier;

    psu->setVoltage(targetVoltage);
    psu->setCurrent(targetCurrent);
}
