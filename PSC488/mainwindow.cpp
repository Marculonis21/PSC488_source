#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <iostream>
#include <qnamespace.h>
#include <qobject.h>
#include <qstandarditemmodel.h>
#include <qvalidator.h>

#include "psu.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->monitorAmps->display(173.20);
    ui->monitorVolts->display(178.11);

    ui->currentEdit->setValidator(new QDoubleValidator(0.0, 200.0, 2));

    this->psu = std::make_unique<Psu>(ui->textBrowser);
    /* this->comboModel = qobject_cast<QStandardItemModel *>(ui->voltageCombo->model()); */
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_currentEdit_textChanged(const QString &text)
{
    std::cout << "current edit change: " << text.toStdString() << std::endl;

    double value = text.toDouble();
    int index = ui->voltageCombo->currentIndex();

    QStandardItemModel* comboModel = qobject_cast<QStandardItemModel *>(ui->voltageCombo->model()); 
    comboModel->item(0)->setEnabled(true);
    comboModel->item(1)->setEnabled(value <= 172.8);
    comboModel->item(2)->setEnabled(value <= 172);
    comboModel->item(3)->setEnabled(value <= 170);

    if (!comboModel->item(index)->isEnabled()) {
        while(!comboModel->item(index)->isEnabled()) {
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

void MainWindow::on_psu_connectButton_clicked()
{
    psu->connect(ui->comboBox->currentText());
}

void MainWindow::on_psu_checkHealthButton_clicked()
{
    psu->checkHealth();
}

void MainWindow::on_pushButton_3_clicked()
{

}

void MainWindow::on_pushButton_4_clicked()
{

}

