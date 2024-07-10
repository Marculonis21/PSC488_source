#ifndef PSUWORKER_HPP
#define PSUWORKER_HPP

#include "plot.hpp"
#include "psu.hpp"
#include <qlcdnumber.h>
#include <qobject.h>

class PsuWorker : public QObject {
  Q_OBJECT;

  public:
    PsuWorker(Psu *psu, Plot *plot, QLCDNumber *currLCD, QLCDNumber *voltLCD, QLCDNumber *currDiffLCD);

    void run();
    void stop();
    void setTargets(Voltage tVoltage, Current tCurrent);

  private:
    Psu *psu;
    Plot *plot;

    QLCDNumber *currLCD;
    QLCDNumber *voltLCD;
    QLCDNumber *currDiffLCD;

    Voltage zero;
    Voltage targetVoltage;
    Current targetCurrent;

    bool running = true;

    int plotEntryID = 0;
    Voltage measVO;
    Current measCU;
    Current lastMeasCU;

    const double voltageMultiplier = 1/1.9;
    const double currentMultiplier = 1/5.3;
    
  public slots:
    void runMeasurement();
    void runDataCollection();

  signals:
    void measure();
    void plotRedraw();
};

#endif
