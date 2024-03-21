#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstandarditemmodel.h>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QStandardItemModel* comboModel;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_currentEdit_textChanged(const QString &text);

    void on_voltageCombo_currentIndexChanged(int index);

    void on_setButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
