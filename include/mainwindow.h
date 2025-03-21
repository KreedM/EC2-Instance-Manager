// This file is part of EC2-Instance-Manager
// Copyright (C) 2025  Derek Meng

// EC2-Instance-Manager is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// EC2-Instance-Manager is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with EC2-Instance-Manager.  If not, see <https://www.gnu.org/licenses/>.

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

    void on_reloadButton_clicked();

    void on_instancesComboBox_currentIndexChanged(int index);

    void on_filterLineEdit_textChanged(const QString &arg1);

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

private:
    Ui::MainWindow *ui;
    EC2InstanceManager manager;

    QString getInstanceID();

    void reloadInstancesComboBox();

    void reloadStateLabel();

    void reloadDescribeTableView();

    void filterDescribeTableView();

    QString currPath = "";

    bool saved, reloadingComboBox;
};
#endif // MAINWINDOW_H
