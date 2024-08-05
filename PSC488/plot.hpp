#ifndef PLOT_H
#define PLOT_H

#include "qcustomplot.h"
#include "psu.hpp"
#include <qvector.h>
class Plot : public QObject
{
    Q_OBJECT;

public:
    Plot(QCustomPlot *plot);
    virtual ~Plot() { 
        delete dataGraph;

        delete customLimitLine;
        delete line1;
        delete line2;
        delete line3;
    }

    void placeData(QVector<double> &&x, QVector<double> &&y);
    void appendData(const QVector<double> &x, const QVector<double> &y);
    void setLimit(const Current limit);
    void drawTest();
    void clear();
    bool save();

private:
    QVector<double> xData;
    QVector<double> yData;

    QCustomPlot *plot;

    QCPGraph *dataGraph;

    QCPItemLine *customLimitLine = nullptr; 
    QCPItemLine *line1; 
    QCPItemLine *line2;
    QCPItemLine *line3;

    void fixRanges(float lower, QCPAxis *axis);

private slots:
    void rangeX(QCPRange newRange);
    void rangeY(QCPRange newRange);

public slots:
    void redraw();
};

#endif
