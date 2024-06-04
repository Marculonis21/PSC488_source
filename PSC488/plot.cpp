#include "plot.hpp"
#include "qcustomplot.h"
#include <cmath>
#include <iostream>
#include <qnamespace.h>
#include <qvector.h>

Plot::Plot(QCustomPlot *plot) {
    float scaleBase = 1.1;
    this->plot = plot;
    this->plot->xAxis->setRange(0, 100);
    this->plot->yAxis->setRange(0, 173);

    this->dataGraph = plot->addGraph();

    line1 = new QCPItemLine(this->plot);
    line2 = new QCPItemLine(this->plot);
    line3 = new QCPItemLine(this->plot);

    line1->setPen(QPen(Qt::black, 1, Qt::DashLine));
    line2->setPen(QPen(Qt::black, 1, Qt::DashLine));
    line3->setPen(QPen(Qt::black, 1, Qt::DashLine));

    line1->start->setCoords(0, 170.0);
    line1->end->setCoords(1000, 170.0);
    line2->start->setCoords(0, 172.0);
    line2->end->setCoords(1000, 172.0);
    line3->start->setCoords(0, 172.8);
    line3->end->setCoords(1000, 172.8);

    this->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QObject::connect(this->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this,
            SLOT(rangeX(QCPRange)));
    QObject::connect(this->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this,
            SLOT(rangeY(QCPRange)));

    clear();
}

void Plot::setLimit(double limit) {
    if (customLimitLine == nullptr) {
        customLimitLine = new QCPItemLine(this->plot);
    }

    customLimitLine->start->setCoords(0, limit);
    customLimitLine->end->setCoords(plot->xAxis->range().upper, limit);

    redraw();
}

void Plot::appendData(const QVector<double> &x, const QVector<double> &y) {
    this->xData.append(x);
    this->yData.append(y);
}

void Plot::placeData(QVector<double> &&x, QVector<double> &&y) {
    this->xData = x;
    this->yData = y;
}

void Plot::clear() {
    this->xData.clear();
    this->yData.clear();

    this->dataGraph->data()->clear();

    redraw();
}

void Plot::redraw() {
    this->dataGraph->setPen(QPen(Qt::red));
    this->dataGraph->setData(xData, yData);
    this->plot->replot();
}

void Plot::fixRanges(float lower, QCPAxis *axis) {
    if (lower < 0) {
        axis->moveRange(-lower);
    }
}

void Plot::rangeX(QCPRange newRange) {
    fixRanges(newRange.lower + 0.01, this->plot->xAxis);

    if (customLimitLine != nullptr) {
        customLimitLine->end->setCoords(newRange.upper+1, customLimitLine->end->coords().y());
    }
    line1->end->setCoords(newRange.upper+1, line1->end->coords().y());
    line2->end->setCoords(newRange.upper+1, line2->end->coords().y());
    line3->end->setCoords(newRange.upper+1, line3->end->coords().y());
}

void Plot::rangeY(QCPRange newRange) {
    fixRanges(newRange.lower + 0.01, this->plot->yAxis);
}
