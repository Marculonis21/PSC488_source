#ifndef PSUWORKER_HPP
#define PSUWORKER_HPP

#include "plot.hpp"
#include "psu.hpp"
#include <qlcdnumber.h>
#include <qobject.h>

class PsuWorker : public QObject {
  Q_OBJECT;

  public:
    PsuWorker(Psu *psu, Plot *plot, QLCDNumber *currLCD, QLCDNumber *voltLCD);

    void run();
    void stop();

  private:
    Psu *psu;
    Plot *plot;

    QLCDNumber *currLCD;
    QLCDNumber *voltLCD;

    Voltage zero;
    Voltage targetVoltage;
    Current targetCurrent;

    bool running = true;

    int plotEntryID = 0;
    Voltage measVO;
    Current measCU;
    

  public slots:
    void runMeasurement();
    void runDataCollection();

  signals:
    void measure();
    void plotRedraw();
};

#endif
