#include "threadWorker.hpp"

Worker::Worker(Plot *plot) { 
    this->plot = plot;
}

Worker::~Worker() { // Destructor
    // free resources
}

void Worker::process() { // Process. Start processing data.
    plot->draw();
    // allocate resources using new here
    emit finished();
}
