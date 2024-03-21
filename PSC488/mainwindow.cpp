#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <iostream>
#include <qnamespace.h>
#include <qobject.h>
#include <qstandarditemmodel.h>
#include <qvalidator.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->monitorAmps->display(173.20);
    ui->monitorVolts->display(178.11);

    ui->currentEdit->setValidator(new QDoubleValidator(0.0, 200.0, 2));

    this->comboModel = qobject_cast<QStandardItemModel *>(ui->voltageCombo->model());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_currentEdit_textChanged(const QString &text)
{
    std::cout << "current edit change: " << text.toStdString() << std::endl;

    double value = text.toDouble();
    int index = ui->voltageCombo->currentIndex();

    this->comboModel->item(0)->setEnabled(true);
    this->comboModel->item(1)->setEnabled(value <= 172.8);
    this->comboModel->item(2)->setEnabled(value <= 172);
    this->comboModel->item(3)->setEnabled(value <= 170);

    if (!this->comboModel->item(index)->isEnabled()) {
        while(!this->comboModel->item(index)->isEnabled()) {
            index--;
        }
        ui->voltageCombo->setCurrentIndex(index);
    }
}

void MainWindow::on_voltageCombo_currentIndexChanged(int index)
{
    std::cout << "combo id change: " << index << std::endl;
}

void MainWindow::on_setButton_clicked()
{
    std::cout << "SET Button Clicked" << std::endl;

    ui->currentShow->setText(ui->currentEdit->text());
    ui->voltageShow->setText(ui->voltageCombo->currentText());
}
