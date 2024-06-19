#ifndef THREADWORKER_H
#define THREADWORKER_H

#include "plot.hpp"
#include "psu.hpp"
#include "qobject.h"
#include "qthread.h"
#include "ui_mainwindow.h"
#include <qlcdnumber.h>
#include <qobjectdefs.h>
#include <qrandom.h>

/* class PlottingTestThread : public QThread { */
/*     Q_OBJECT; */
/*   public: */
/*     PlottingTestThread(Plot *plot, QLCDNumber *currLCD, QLCDNumber *voltLCD)
 * { */
/*         this->plot = plot; */
/*         this->currLCD = currLCD; */
/*         this->voltLCD = voltLCD; */

/*         connect(this, &PlottingTestThread::plotRedraw, plot, &Plot::redraw);
 */
/*     } */

/*     void run() override { */
/*         plot->clear(); */

/*         for (int i = 0; i < 1000; ++i) { */
/*             plot->appendData({static_cast<double>(i)}, */
/*                              {static_cast<double>(i)}); */
/*             currLCD->display(i); */
/*             voltLCD->display(i); */
/*             emit plotRedraw(); */
/*             QThread::msleep(10); */
/*         } */

/*         emit resultReady(); */
/*     } */

/*   private: */
/*     Plot *plot; */
/*     QLCDNumber *currLCD; */
/*     QLCDNumber *voltLCD; */

/*   signals: */
/*     void resultReady(); */
/*     void plotRedraw(); */
/* }; */

class LiveMeasurementThread : public QThread {
    Q_OBJECT;

  public:
    LiveMeasurementThread(Plot *plot, Psu *psu, QLCDNumber *currLCD,
                          QLCDNumber *voltLCD);
    void run() override;
    void stopMeasurement();

  private:
    Plot *plot;
    Psu *psu;
    QLCDNumber *currLCD;
    QLCDNumber *voltLCD;

    bool running = true;

  signals:
    void endSignal();
    void plotRedraw();
    void measure();

};

class PsuPowerThread : public QThread {
    Q_OBJECT;

  public:
    PsuPowerThread(Psu *psu, Voltage targetVoltage, Current targetCurrent);

    void run() override;
    void changeTarget(Voltage voltage, Current current);
    void stopPower();

  private:
    Psu *psu;

    Voltage targetVoltage;
    Current targetCurrent;

    bool running = true;

    void setPsuTargets();

  signals:
    void endSignal();
    void plotRedraw();
    void setVoltage(Voltage voltage);
    void setCurrent(Current current);
};

#endif
