#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("EC2 Instance Manager");


    QStandardItemModel *model = new QStandardItemModel();
    ui->describeTableView->setModel(model);
    //ui->describeTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //ui->describeTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->describeTableView->horizontalHeader()->setResizeContentsPrecision(-1);
    ui->describeTableView->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getInstanceID() {
    std::string instanceID = ui->instancesComboBox->currentText().toStdString();

    int idEnd = instanceID.find(" (");

    if(idEnd == std::string::npos)
        return QString::fromStdString(instanceID);
    else
        return QString::fromStdString(instanceID.substr(0, idEnd));
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

    this->saved = false;
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
    ((QStandardItemModel*)ui->describeTableView->model())->clear();

    ui->stateLabelActual->clear();
    ui->statusbar->showMessage("Removed instance " + instanceID + "!", 5000);
    qDebug().nospace().noquote() << "REMOVED instance " << instanceID << "!\n";

    this->saved = false;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
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

void MainWindow::reloadStateLabel() {
    const QString& state = manager.getEC2InstanceStatus(getInstanceID());
    QString color = "gray";

    if(state == "running")
        color = "green";
    else if(state == "stopping")
        color = "orange";
    else if(state == "stopped" || state == "shutting-down")
        color = "red";
    else if(state == "terminated")
        color = "maroon";

    ui->stateLabelActual->setStyleSheet("QLabel {color : " + color + ";}");
    ui->stateLabelActual->setText(state.toUpper());
}

void MainWindow::on_filterLineEdit_textChanged(const QString &arg1)
{
    filterDescribeTableView();
}

void MainWindow::filterDescribeTableView() {
    const QString& filter = ui->filterLineEdit->text().toLower();

    QStandardItemModel *model = (QStandardItemModel*)ui->describeTableView->model();
    for(int i = 0; i < model->rowCount(); ++i) {
        ui->describeTableView->setRowHidden(i, true);

        for(int j = 0; j < model->columnCount(); ++j) {
            QStandardItem *item = model->item(i, j);

            if(item->text().toLower().contains(filter)) {
                ui->describeTableView->setRowHidden(i, false);
                break;
            }
        }
    }

    ui->describeTableView->resizeColumnToContents(0);
}

void MainWindow::reloadDescribeTableView() {
    const QString& instanceID = getInstanceID();

    if(instanceID.isEmpty())
        return;

    QStandardItemModel *model = (QStandardItemModel*)ui->describeTableView->model();
    model->clear();

    std::vector<std::pair<QString, QString>> descriptions;

    const QString& describeInstanceResult = manager.describeEC2Instance(instanceID, descriptions,
                                                                        ui->instanceSummaryCheckBox->isChecked(), ui->instanceDetailsCheckBox->isChecked(),
                                                                        ui->hostAndPlacementGroupCheckBox->isChecked(), ui->capacityReservationCheckBox->isChecked());

    if(!describeInstanceResult.isEmpty()) {
        QMessageBox::critical(this, "EC2 Instance Manager", describeInstanceResult);
        qDebug().nospace().noquote() << describeInstanceResult << "\n";
        return;
    }

    if(descriptions.empty())
        return;

    ui->instancesComboBox->setItemText(ui->instancesComboBox->currentIndex(), instanceID + " (" + manager.getEC2InstanceName(instanceID) + ")");

    reloadStateLabel();

    model->setRowCount(descriptions.size()); model->setColumnCount(2);
    for(int i = 0; i < descriptions.size(); ++i) {
        model->setItem(i, 0, new QStandardItem(descriptions[i].first));
        model->setItem(i, 1, new QStandardItem(descriptions[i].second));
    }

    filterDescribeTableView();

    ui->statusbar->showMessage("Described instance " + instanceID + "!", 5000);
    qDebug().nospace().noquote() << "DESCRIBED instance " << instanceID << "!\n";

    this->saved = false;
}

void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open", QDir::homePath(), "Instance List File (*.json)");

    if(filePath.isEmpty())
        return;

    qDebug() << "OPENING JSON FILE\n";

    QFile file(filePath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug().nospace() << "Failed to open " << filePath << "!\n";
        QMessageBox::critical(this, "EC2 Instance Manager", "Failed to open \"" + filePath + "\"!");
        return;
    }

    QJsonParseError error;
    const QJsonDocument& document = QJsonDocument::fromJson(file.readAll(), &error);
    if(document.isNull()) {
        qDebug().nospace() << "Failed to parse " << filePath << "!\n";
        QMessageBox::critical(this, "EC2 Instance Manager", "Failed to parse \"" + filePath + "\": " + error.errorString() + "!");
        return;
    }

    if(!document.isArray()) {
        qDebug().nospace() << "Invalid Json file: " << filePath << "!\n";
        QMessageBox::critical(this, "EC2 Instance Manager", "Invalid Json file: \"" + filePath + "\"!");
        return;
    }
    const QJsonArray& array = document.array();

    std::vector<std::pair<QString, QString>> descriptions;
    for(const QJsonValue& value : array) { // First for loop to validate Json file
        if(!value.isObject()) {
            qDebug().nospace() << "Invalid Json file: " << filePath << "!\n";
            QMessageBox::critical(this, "EC2 Instance Manager", "Invalid Json file: \"" + filePath + "\"!");
            return;
        }

        const QJsonObject& object = value.toObject();
        if(!object.contains("id") || !object.contains("name")) {
            qDebug().nospace() << "Invalid Json file: " << filePath << "!\n";
            QMessageBox::critical(this, "EC2 Instance Manager", "Invalid Json file: \"" + filePath + "\"!");
            return;
        }

        const QJsonValue& id = object.value("id"), name = object.value("name");
        if(!id.isString() || !name.isString()) {
            qDebug().nospace() << "Invalid Json file: " << filePath << "!\n";
            QMessageBox::critical(this, "EC2 Instance Manager", "Invalid Json file: \"" + filePath + "\"!");
            return;
        }
    }

    manager.clearEC2Instances();
    for(const QJsonValue& value : array) {
        const QJsonObject& object = value.toObject();
        const QJsonValue& id = object.value("id"), name = object.value("name");

        manager.addEC2Instance(id.toString());
        manager.setEC2InstanceName(id.toString(), name.toString());
    }

    reloadInstancesComboBox();
    ((QStandardItemModel*)ui->describeTableView->model())->clear();

    ui->statusbar->showMessage("Loaded \"" + filePath + "\"!", 5000);

    this->currPath = filePath;
    this->saved = true;
    file.close();
}

void MainWindow::on_actionSave_As_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save as", QDir::homePath(), "Instance List File (*.json)");

    if(filePath.isEmpty())
        return;

    qDebug() << "SAVING TO JSON FILE\n";

    QFile file(filePath);
    if(!file.open(QFile::WriteOnly)) {
        qDebug().nospace() << "Failed to save to " << filePath << "!\n";
        QMessageBox::critical(this, "EC2 Instance Manager", "Failed to save to \"" + filePath + "\"!");
        return;
    }

    QJsonArray instances;
    const std::vector<std::pair<QString, QString>>& instancesList = manager.saveEC2Instances();
    for(const std::pair<QString, QString> instance : instancesList) {
        QJsonObject instanceObject;
        instanceObject.insert("id", QJsonValue(instance.first));
        instanceObject.insert("name", QJsonValue(instance.second));

        instances.append(instanceObject);
    }

    QJsonDocument instancesJson(instances);
    QTextStream textStream(&file);
    textStream << instancesJson.toJson();

    ui->statusbar->showMessage("Saved instance list to \"" + filePath + "\"!", 5000);

    this->currPath = filePath;
    this->saved = true;
    file.close();
}

void MainWindow::on_actionSave_triggered()
{
    if(this->currPath.isEmpty())
        return;

    if(this->saved)
        return;

    if(QMessageBox::question(this, "EC2 Instance Manager", "Save instance list \"" + this->currPath + "\"?") == QMessageBox::No)
        return;

    qDebug() << "SAVING JSON FILE\n";

    QFile file(currPath);
    if(!file.open(QFile::WriteOnly)) {
        qDebug().nospace() << "Failed to save " << currPath << "!\n";
        QMessageBox::critical(this, "EC2 Instance Manager", "Failed to save \"" + currPath + "\"!");
        return;
    }

    QJsonArray instances;
    const std::vector<std::pair<QString, QString>>& instancesList = manager.saveEC2Instances();
    for(const std::pair<QString, QString> instance : instancesList) {
        QJsonObject instanceObject;
        instanceObject.insert("id", QJsonValue(instance.first));
        instanceObject.insert("name", QJsonValue(instance.second));

        instances.append(instanceObject);
    }

    QJsonDocument instancesJson(instances);
    QTextStream textStream(&file);
    textStream << instancesJson.toJson();

    ui->statusbar->showMessage("Saved instance list \"" + currPath + "\"!", 5000);

    this->saved = true;
    file.close();
}

