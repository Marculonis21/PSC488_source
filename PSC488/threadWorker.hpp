#ifndef THREADWORKER_H
#define THREADWORKER_H

#include "plot.hpp"
#include "qobject.h"
#include "qthread.h"

class Worker : public QObject {
    Q_OBJECT
public:
    Worker(Plot *plot);
    ~Worker();
public slots:
    void process();
signals:
    void finished();
    void error(QString err);
private:

    Plot *plot;
};
#endif // THREADINGTEST_H
