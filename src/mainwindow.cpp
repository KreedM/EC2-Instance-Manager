#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("EC2 Instance Manager");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getInstanceID() {
    return ui->instancesComboBox->currentText();
}

void MainWindow::on_startButton_clicked()
{
    const QString instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    const QString result = manager.startEC2Instance(instanceID);

    if(result.isEmpty()) {
        QMessageBox::information(this, " ", "Started instance " + instanceID + "!");
        ui->statusbar->showMessage("Started instance " + instanceID + "!", 5000);
        qDebug().nospace().noquote() << "STARTED instance " << instanceID << "!\n";
    }
    else {
        QMessageBox::critical(this, " ", result + "\n");
        qDebug().nospace().noquote() << result << "\n";
    }
}


void MainWindow::on_stopButton_clicked()
{
    const QString instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    const QString result = manager.stopEC2Instance(instanceID);

    if(result.isEmpty()) {
        QMessageBox::information(this, " ", "Stopped instance " + instanceID + "!");
        ui->statusbar->showMessage("Stopped instance " + instanceID + "!", 5000);
        qDebug().nospace().noquote() << "STOPPED instance " << instanceID << "!\n";
    }
    else {
        QMessageBox::critical(this, " ", result + "\n");
        qDebug().nospace().noquote() << result << "\n";
    }
}


void MainWindow::on_rebootButton_clicked()
{
    const QString instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    const QString result = manager.rebootEC2Instance(instanceID);

    if(result.isEmpty()) {
        QMessageBox::information(this, " ", "Rebooted instance " + instanceID + "!");
        ui->statusbar->showMessage("Rebooted instance " + instanceID + "!", 5000);
        qDebug().nospace().noquote() << "REBOOTED instance " << instanceID << "!\n";
    }
    else {
        QMessageBox::critical(this, " ", result + "\n");
        qDebug().nospace().noquote() << result << "\n";
    }
}

void MainWindow::showInstances() {
    ui->instancesComboBox->clear();

    const std::vector<QString> instancesList = manager.listEC2Instances();

    for(const QString& instance : instancesList)
        ui->instancesComboBox->addItem(instance);

    ui->instancesComboBox->clearEditText();
}

void MainWindow::on_addButton_clicked()
{
    const QString instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    if(manager.contains(instanceID)) {
        QMessageBox::warning(this, " ", "Instance " + instanceID + " already exists!");
        return;
    }

    if(QMessageBox::question(this, " ", "Add instance " + instanceID + "?") == QMessageBox::No)
        return;

    manager.addEC2Instance(instanceID);

    showInstances();
    ui->instancesComboBox->setCurrentIndex(ui->instancesComboBox->findText(instanceID));

    saved = false;

    ui->statusbar->showMessage("Added instance " + instanceID + "!", 5000);
    qDebug().nospace().noquote() << "ADDED instance " << instanceID << "!\n";
}


void MainWindow::on_removeButton_clicked()
{
    const QString instanceID = getInstanceID();

    if(!manager.contains(instanceID))
        return;

    if(QMessageBox::warning(this, " ", "Remove instance " + instanceID + "?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
        return;

    manager.removeEC2Instance(instanceID);

    showInstances();

    this->saved = false;

    ui->statusbar->showMessage("Removed instance " + instanceID + "!", 5000);
    qDebug().nospace().noquote() << "REMOVED instance " << instanceID << "!\n";
}


void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

