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

class PlottingTestThread : public QThread {
  Q_OBJECT;

  Plot *plot;
  QLCDNumber *currLCD;
  QLCDNumber *voltLCD;
public:
  PlottingTestThread(Plot *plot, QLCDNumber *currLCD, QLCDNumber *voltLCD) {
    this->plot = plot;
    this->currLCD = currLCD;
    this->voltLCD = voltLCD;

    connect(this, &PlottingTestThread::plotRedraw, plot, &Plot::redraw);
  }

  void run() override { 
    plot->clear();

    for (int i = 0; i < 1000; ++i) {
        plot->appendData({static_cast<double>(i)}, {static_cast<double>(i)});
        /* plot->redraw(); */
        currLCD->display(i);
        voltLCD->display(i);
        emit plotRedraw();
        QThread::msleep(10);
    }

    emit resultReady(); 
  }
signals:
  void resultReady();
  void plotRedraw();
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

    connect(this, &LiveMeasurementThread::plotRedraw, plot, &Plot::redraw);
  }

  void run() override { 
    double measVO = 0;
    double measCU = 0;

    int entryID = 0;

    while (running) {
      /* measVO = this->psu->measurePSUVoltage(); */
      /* measCU = this->psu->measurePSUCurrent(); */

      measCU = QRandomGenerator::global()->bounded(0,172);
      measVO = QRandomGenerator::global()->bounded(0,5);

      currLCD->display(measCU);
      voltLCD->display(measVO);

      plot->appendData({(double)entryID}, 
                       {measCU});

      emit plotRedraw();
      entryID++;

      QThread::msleep(25);
    }

    emit resultReady(); 
  }

signals:
  void resultReady();
  void plotRedraw();
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
