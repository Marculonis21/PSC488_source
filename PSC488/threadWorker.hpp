#ifndef THREADWORKER_H
#define THREADWORKER_H

#include "plot.hpp"
#include "psu.hpp"
#include "qobject.h"
#include "qthread.h"
#include "ui_mainwindow.h"
#include <qlcdnumber.h>
#include <qrandom.h>

class PlottingTestThread : public QThread {
  Q_OBJECT;

  Plot *plot;

public:
  PlottingTestThread(Plot *plot) {
    this->plot = plot;
  }

  void run() override { 
    plot->drawTest();

    emit resultReady(); 
  }
signals:
  void resultReady();
};

class LiveMeasurementThread : public QThread {
  Q_OBJECT;

  Plot *plot;
  Psu *psu;
  QLCDNumber *currLCD;
  QLCDNumber *voltLCD;

public:
  bool running = true;

  LiveMeasurementThread(Plot *plot, Psu *psu, QLCDNumber *currLCD, QLCDNumber *voltLCD) {
    this->plot = plot;
    this->psu = psu;
    this->currLCD = currLCD;
    this->voltLCD = voltLCD;
    this->plot->clear();
    running = true;
  }

  void run() override { 
    double measVO = 0;
    double measCU = 0;

    int entryID = 0;

    while (running) {
      /* measVO = this->psu->measurePSUVoltage(); */
      /* measCU = this->psu->measurePSUCurrent(); */

      measCU = QRandomGenerator::global()->bounded(0,172.0);
      measVO = QRandomGenerator::global()->bounded(0,5.0);

      currLCD->display(measCU);
      voltLCD->display(measVO);

      plot->appendData({(double)entryID}, 
                       {measCU});

      plot->redraw();

      entryID++;

      QThread::msleep(250);
    }

    emit resultReady(); 
  }
signals:
  void resultReady();
};

/* class Worker : public QObject { */
/*   Q_OBJECT */
/* public: */
/*   Worker(Plot *plot); */
/*   ~Worker(); */
/* public slots: */
/*   void process(); */
/* signals: */
/*   void finished(); */
/*   void error(QString err); */

/* private: */
/*   Plot *plot; */
/* }; */

#endif 
