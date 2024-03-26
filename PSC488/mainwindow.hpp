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
    void on_psu_connectButton_clicked();
    void on_psu_checkHealthButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();

};

#endif // MAINWINDOW_H
