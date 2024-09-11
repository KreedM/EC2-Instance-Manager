#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "EC2InstanceManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_rebootButton_clicked();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    EC2InstanceManager manager;

    void showInstances();

    QString getInstanceID();

    bool saved;
};
#endif // MAINWINDOW_H
