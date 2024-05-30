#include "mainwindow.hpp"
#include "plot.hpp"
#include "threadWorker.hpp"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
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
  delete plottingThread;
  this->plottingThread = nullptr;
}

void MainWindow::on_drawTestButton_clicked() {
  std::cout << "hey2" << std::endl;
  /* this->plot->draw(); */

  if (this->plottingThread != nullptr)
    return;

  this->plottingThread = new PlottingThread(this->plot);
  connect(plottingThread, &PlottingThread::resultReady, this,
          &MainWindow::plottingDone);
  plottingThread->start();

  std::cout << "Thread started" << std::endl;
}

void MainWindow::on_currentEdit_textChanged(const QString &text) {
  std::cout << "current edit change: " << text.toStdString() << std::endl;

  double value = text.toDouble();
  int index = ui->voltageCombo->currentIndex();

  QStandardItemModel *comboModel =
      qobject_cast<QStandardItemModel *>(ui->voltageCombo->model());
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
}

// PSU CONNECTION
void MainWindow::on_psu_conn_connect_clicked() {
  psu->connect(ui->portCombo->currentText());
}
void MainWindow::on_psu_conn_checkHealth_clicked() { psu->checkHealth(); }
void MainWindow::on_psu_conn_help_clicked() { auto out = psu->query("HELP"); }

// PSU MEASUREMENTS
void MainWindow::on_psu_meas_sourceVolt_clicked() {
  auto out = psu->query("SO:VO");
}
void MainWindow::on_psu_meas_measureVolt_clicked() {
  auto out = psu->query("ME:VO");
}
void MainWindow::on_psu_meas_sourceCurr_clicked() {
  auto out = psu->query("SO:CU");
}
void MainWindow::on_psu_meas_measureCurr_clicked() {
  auto out = psu->query("ME:CU");
}
void MainWindow::on_psu_meas_maxVolt_clicked() {
  auto out = psu->query("SO:VO:MA");
}
void MainWindow::on_psu_meas_maxCurr_clicked() {
  auto out = psu->query("SO:CU:MA");
}

// PSU ON OFF FUNCS
void MainWindow::on_psu_psu_funcOutputQ_clicked() {
  auto out = psu->query("SO:FU:RSD").trimmed().toStdString();

  if (out == "1") ui->textBrowser->insertPlainText("RSD enabled, output disabled\n");
  else            ui->textBrowser->insertPlainText("RSD disabled, output enabled\n");
}

void MainWindow::on_psu_psu_funcOutputSwitch_clicked() { psu->powerSwitch(); }

// PSU REMOTE/LOCAL
void MainWindow::on_psu_conn_localQ_clicked() { psu->set("LOC"); }
void MainWindow::on_psu_conn_remoteQ_clicked() { psu->set("REM"); }
void MainWindow::on_psu_conn_remoteLocalSwitch_clicked() {
  auto out = psu->query("REM?").trimmed().toStdString();
  std::cout << out << std::endl;

  if (out == "1") ui->textBrowser->insertPlainText("PSU IS REMOTE\n");
  else            ui->textBrowser->insertPlainText("PSU IS LOCAL\n");
}

// CUSTOM COMMANDS
void MainWindow::on_customCommandSendButton_clicked() {
    // get command from input

    if (ui->customCommandInput->text().isEmpty()) return;

    std::string command = ui->customCommandInput->text().trimmed().toStdString();
    ui->customCommandInput->clear();

    psu->set(command);
}

