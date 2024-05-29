#ifndef THREADWORKER_H
#define THREADWORKER_H

#include "plot.hpp"
#include "qobject.h"
#include "qthread.h"

class PlottingThread : public QThread {
  Q_OBJECT

  Plot *plot;
public:
  PlottingThread(Plot *plot) {
    this->plot = plot;
  }

  void run() override { 
    plot->draw();

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
