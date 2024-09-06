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

private:
    Ui::MainWindow *ui;
    EC2InstanceManager manager;
};
#endif // MAINWINDOW_H
