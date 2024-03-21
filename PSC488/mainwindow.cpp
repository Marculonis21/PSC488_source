#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->monitorAmps->display(173.20);
    ui->monitorVolts->display(178.11);
    std::cout << "run" << std::endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_currentEdit_textChanged(const QString &arg1)
{

}


void MainWindow::on_voltageCombo_currentIndexChanged(int index)
{

}


void MainWindow::on_setButton_clicked()
{

}

