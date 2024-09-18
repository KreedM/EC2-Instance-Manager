#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QStandardItemModel>

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

void MainWindow::reloadDescribeTableView() {
    const QString& instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    std::vector<std::pair<QString, QString>> descriptions;
    const QString& result = manager.describeEC2Instance(instanceID, descriptions);

    if(!result.isEmpty()) {
        QMessageBox::critical(this, "EC2 Instance Manager", result);
        qDebug().nospace().noquote() << result << "\n";
        return;
    }

    if(!manager.getEC2InstanceName(instanceID).isEmpty())
        ui->instancesComboBox->setItemText(ui->instancesComboBox->currentIndex(), instanceID + " (" + manager.getEC2InstanceName(instanceID) + ")");

    QStandardItemModel *model = new QStandardItemModel(descriptions.size(), 2);
    for(int i = 0; i < descriptions.size(); ++i) {
        model->setItem(i, 0, new QStandardItem(descriptions[i].first));
        model->setItem(i, 1, new QStandardItem(descriptions[i].second));
    }

    ui->describeTableView->setModel(model);
    //ui->describeTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //ui->describeTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->describeTableView->resizeColumnToContents(0);
    ui->describeTableView->horizontalHeader()->setResizeContentsPrecision(-1);
    ui->describeTableView->horizontalHeader()->setStretchLastSection(true);

    ui->statusbar->showMessage("Described instance " + instanceID + "!", 5000);
    qDebug().nospace().noquote() << "DESCRIBED instance " << instanceID << "!\n";
}

void MainWindow::on_instancesComboBox_currentIndexChanged(int index)
{
    if(reloadingComboBox || index == -1)
        return;

    qDebug().noquote().nospace() << "INDEX changed to " << index << "!\n";
    reloadDescribeTableView();
}

void MainWindow::on_reloadButton_clicked()
{
    const QString& instanceID = getInstanceID();

    if(!manager.contains(instanceID))
        return;

    qDebug().noquote().nospace() << "RELOAD called!\n";
    reloadDescribeTableView();
}

void MainWindow::on_startButton_clicked()
{
    const QString& instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    if(QMessageBox::question(this, "EC2 Instance Manager", "Start instance " + instanceID + "?") == QMessageBox::No)
        return;

    const QString& result = manager.startEC2Instance(instanceID);

    if(result.isEmpty()) {
        QMessageBox::information(this, "EC2 Instance Manager", "Started instance " + instanceID + "!");
        ui->statusbar->showMessage("Started instance " + instanceID + "!", 5000);
        qDebug().nospace().noquote() << "STARTED instance " << instanceID << "!\n";
    }
    else {
        QMessageBox::critical(this, "EC2 Instance Manager", result);
        qDebug().nospace().noquote() << result << "\n";
    }
}


void MainWindow::on_stopButton_clicked()
{
    const QString& instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    if(QMessageBox::question(this, "EC2 Instance Manager", "Stop instance " + instanceID + "?") == QMessageBox::No)
        return;

    const QString& result = manager.stopEC2Instance(instanceID);

    if(result.isEmpty()) {
        QMessageBox::information(this, "EC2 Instance Manager", "Stopped instance " + instanceID + "!");
        ui->statusbar->showMessage("Stopped instance " + instanceID + "!", 5000);
        qDebug().nospace().noquote() << "STOPPED instance " << instanceID << "!\n";
    }
    else {
        QMessageBox::critical(this, "EC2 Instance Manager", result);
        qDebug().nospace().noquote() << result << "\n";
    }
}


void MainWindow::on_rebootButton_clicked()
{
    const QString& instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    if(QMessageBox::question(this, "EC2 Instance Manager", "Reboot instance " + instanceID + "?") == QMessageBox::No)
        return;

    const QString& result = manager.rebootEC2Instance(instanceID);

    if(result.isEmpty()) {
        QMessageBox::information(this, "EC2 Instance Manager", "Rebooted instance " + instanceID + "!");
        ui->statusbar->showMessage("Rebooted instance " + instanceID + "!", 5000);
        qDebug().nospace().noquote() << "REBOOTED instance " << instanceID << "!\n";
    }
    else {
        QMessageBox::critical(this, "EC2 Instance Manager", result);
        qDebug().nospace().noquote() << result << "\n";
    }
}

void MainWindow::reloadInstancesComboBox() {
    reloadingComboBox = true;

    ui->instancesComboBox->clear();

    const std::vector<QString>& instancesList = manager.listEC2Instances();

    for(const QString& instance : instancesList)
        ui->instancesComboBox->addItem(instance);

    ui->instancesComboBox->clearEditText();
    ui->instancesComboBox->setCurrentIndex(-1);

    reloadingComboBox = false;

    qDebug().nospace().noquote() << "SHOWED instances in combo box!\n";
}

void MainWindow::on_addButton_clicked()
{
    const QString& instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    if(manager.contains(instanceID)) {
        QMessageBox::warning(this, "EC2 Instance Manager", "Instance " + instanceID + " already exists!");
        return;
    }

    if(QMessageBox::question(this, "EC2 Instance Manager", "Add instance " + instanceID + "?") == QMessageBox::No)
        return;

    manager.addEC2Instance(instanceID);

    ui->statusbar->showMessage("Added instance " + instanceID + "!", 5000);
    qDebug().nospace().noquote() << "ADDED instance " << instanceID << "!\n";

    reloadInstancesComboBox();
    ui->instancesComboBox->setCurrentIndex(ui->instancesComboBox->findText(instanceID));

    saved = false;
}


void MainWindow::on_removeButton_clicked()
{
    const QString& instanceID = getInstanceID();

    if(!manager.contains(instanceID))
        return;

    if(QMessageBox::question(this, "EC2 Instance Manager", "Remove instance " + instanceID + "?") == QMessageBox::No)
        return;

    manager.removeEC2Instance(instanceID);

    reloadInstancesComboBox();
    //describeInstances(); //To clear out instance info

    saved = false;

    ui->statusbar->showMessage("Removed instance " + instanceID + "!", 5000);
    qDebug().nospace().noquote() << "REMOVED instance " << instanceID << "!\n";
}


void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

