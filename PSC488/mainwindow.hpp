#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstandarditemmodel.h>
#include <vector>
#include <memory>

#include "psu.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE;



class MainWindow : public QMainWindow
{
    Q_OBJECT;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<Psu> psu;

private slots:
    void on_currentEdit_textChanged(const QString &text);
    void on_voltageCombo_currentIndexChanged(int index);

    void on_setButton_clicked();
    void on_psu_conn_connect_clicked();
    void on_psu_conn_checkHealth_clicked();
    void on_psu_conn_help_clicked();
    void on_psu_conn_localQ_clicked();
    void on_psu_conn_remoteLocalSwitch_clicked();
    void on_psu_conn_remoteQ_clicked();
    void on_psu_meas_sourceVolt_clicked();
    void on_psu_meas_measureVolt_clicked();
    void on_psu_meas_sourceCurr_clicked();
    void on_psu_meas_measureCurr_clicked();
    void on_psu_meas_maxVolt_clicked();
    void on_psu_meas_maxCurr_clicked();
    void on_psu_psu_funcOutputQ_clicked();
    void on_psu_psu_funcOutputSwitch_clicked();
};

#endif // MAINWINDOW_H
