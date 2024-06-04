#include "mainwindow.hpp"
#include "plot.hpp"
#include "threadWorker.hpp"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <cstdio>
#include <iostream>
#include <qnamespace.h>
#include <qobject.h>
#include <qserialportinfo.h>
#include <qstandarditemmodel.h>
#include <qthread.h>
#include <qvalidator.h>
#include <string>

#include "psu.hpp"

const double topLimit = 172.8;
const double midLimit = 172.0;
const double lowLimit = 170.0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    ui->monitorAmps->display(173.20);
    ui->monitorVolts->display(178.11);

    ui->baudCombo->setCurrentIndex(2);
    ui->baudCombo->setEnabled(false);

    ui->currentEdit->setValidator(new QDoubleValidator(0.0, 200.0, 2));

    this->psu = std::make_unique<Psu>(ui->textBrowser);

    on_currentEdit_textChanged(ui->currentEdit->text());

    // find com ports
    refreshPorts();

    // setup plotting
    this->plot = new Plot(ui->plotPanel);

    // PSU output buttons colors
    ui->outputOnButton->setStyleSheet(QString(
        "QPushButton:enabled{background-color: rgb(119,221,119);}"
        "QPushButton:disabled{}"
        "QPushButton:hover:!pressed{background-color: rgb(149, 221, 149);}"));

    ui->outputOffButton->setStyleSheet(QString(
        "QPushButton:enabled{background-color: rgb(255,105,97);}"
        "QPushButton:disabled{}"
        "QPushButton:hover:!pressed{background-color: rgb(255,135,127);}"));

    ui->outputOnButton->setEnabled(false);
    ui->outputOffButton->setEnabled(false);
    /* ui->setButton->setEnabled(false); */
}

MainWindow::~MainWindow() {}

void MainWindow::refreshPorts() {
    ui->portCombo->clear();
    for (QSerialPortInfo port : QSerialPortInfo::availablePorts()) {
        ui->portCombo->addItem(port.portName());
        std::cout << "port: " << port.portName().toStdString() << std::endl;
    }
}

void MainWindow::on_refreshButton_clicked() { refreshPorts(); }

void MainWindow::plottingDone() {
    /* delete plottingThread; */
    /* this->plottingThread = nullptr; */

    delete liveMeasThread;
    this->liveMeasThread = nullptr;
}

void MainWindow::on_drawTestButton_clicked() {
    std::cout << "hey2" << std::endl;
    /* this->plot->draw(); */

    if (liveMeasThread) {
        liveMeasThread->running = false;
        return;
    }
    this->liveMeasThread = new LiveMeasurementThread(this->plot, this->psu.get(), ui->monitorAmps, ui->monitorVolts);
    connect(liveMeasThread, &LiveMeasurementThread::resultReady, this, &MainWindow::plottingDone);
    liveMeasThread->start();

    /* if (plottingThread) return; */
    /* this->plottingThread = new PlottingTestThread(this->plot, ui->monitorAmps, ui->monitorVolts); */
    /* connect(plottingThread, &PlottingTestThread::resultReady, this, &MainWindow::plottingDone); */
    /* plottingThread->start(); */


    std::cout << "Thread started" << std::endl;
}

void MainWindow::on_currentEdit_textChanged(const QString &text) {
    std::cout << "current edit change: " << text.toStdString() << std::endl;

    double value = text.toDouble();
    int index = ui->voltageCombo->currentIndex();

    QStandardItemModel *comboModel = qobject_cast<QStandardItemModel *>(ui->voltageCombo->model());
    comboModel->item(0)->setEnabled(true);
    comboModel->item(1)->setEnabled(value <= topLimit);
    comboModel->item(2)->setEnabled(value <= midLimit);
    comboModel->item(3)->setEnabled(value <= lowLimit);

    if (!comboModel->item(index)->isEnabled()) {
        while (!comboModel->item(index)->isEnabled()) {
            index--;
        }
        ui->voltageCombo->setCurrentIndex(index);
    }
}

void MainWindow::on_voltageCombo_currentIndexChanged(int index) {
    std::cout << "combo id change: " << index << std::endl;
}

void MainWindow::on_setButton_clicked() {
    std::cout << "SET Button Clicked" << std::endl;

    ui->currentShow->setText(ui->currentEdit->text());
    ui->voltageShow->setText(ui->voltageCombo->currentText());

    bool cOk, vOk;
    double current = ui->currentEdit->text().toDouble(&cOk);
    double voltage = ui->voltageCombo->currentText().chopped(1).toDouble(&vOk);

    if (!cOk || !vOk) {
        printf("Problem while parsing out current/voltage values from input "
               "(SET STOPPED) - %f, %f\n", current, voltage);
        return;
    }


    printf("%f, %f\n", current, voltage);

/*     psu->setVoltage(voltage); */
/*     QThread::msleep(500); */
/*     psu->setCurrent(current); */

    plot->setLimit(current);
}

// YES IT IS CORRECT (famous last words...)
//
// RSD OFF - disables RSD -> enables output
// RSD ON  - enables RSD  -> disables output
void MainWindow::on_outputOnButton_clicked() { psu->set("SO:FU:RSD", "OFF"); }
void MainWindow::on_outputOffButton_clicked() { psu->set("SO:FU:RSD", "ON"); }

// PSU CONNECTION
void MainWindow::on_psu_conn_connect_clicked() {
    psu->connect(ui->portCombo->currentText());

    if (psu->isConnected()) {
        ui->outputOnButton->setEnabled(!psu->isTurnedOn());
        ui->outputOffButton->setEnabled(psu->isTurnedOn());
        ui->setButton->setEnabled(psu->isConnected());
    }
}

void MainWindow::on_psu_conn_checkHealth_clicked() { psu->checkHealth(); }
void MainWindow::on_psu_conn_help_clicked() { auto out = psu->query("HELP"); }

// PSU MEASUREMENTS
void MainWindow::on_psu_meas_sourceVolt_clicked()  { auto out = psu->query("SO:VO"); }
void MainWindow::on_psu_meas_measureVolt_clicked() { auto out = psu->query("ME:VO"); }
void MainWindow::on_psu_meas_sourceCurr_clicked()  { auto out = psu->query("SO:CU"); }
void MainWindow::on_psu_meas_measureCurr_clicked() { auto out = psu->query("ME:CU"); }
void MainWindow::on_psu_meas_maxVolt_clicked() { auto out = psu->query("SO:VO:MA"); }
void MainWindow::on_psu_meas_maxCurr_clicked() { auto out = psu->query("SO:CU:MA"); }

// PSU ON OFF FUNCS
void MainWindow::on_psu_psu_funcOutputQ_clicked() {
    auto out = psu->query("SO:FU:RSD").trimmed().toStdString();

    if (out == "1")
        ui->textBrowser->insertPlainText("RSD enabled, output disabled\n");
    else
        ui->textBrowser->insertPlainText("RSD disabled, output enabled\n");
}

void MainWindow::on_psu_psu_funcOutputSwitch_clicked() { psu->powerSwitch(); }

// PSU REMOTE/LOCAL
void MainWindow::on_psu_conn_localQ_clicked() { psu->set("LOC"); }
void MainWindow::on_psu_conn_remoteQ_clicked() { psu->set("REM"); }
void MainWindow::on_psu_conn_remoteLocalSwitch_clicked() {
    auto out = psu->query("REM?").trimmed().toStdString();
    std::cout << out << std::endl;

    if (out == "1")
        ui->textBrowser->insertPlainText("PSU IS REMOTE\n");
    else
        ui->textBrowser->insertPlainText("PSU IS LOCAL\n");
}

// CUSTOM COMMANDS
void MainWindow::on_customCommandSendButton_clicked() {
    if (ui->customCommandInput->text().isEmpty()) return;

    std::string command = ui->customCommandInput->text().trimmed().toStdString();
    ui->customCommandInput->clear();

    psu->set(command);
}

