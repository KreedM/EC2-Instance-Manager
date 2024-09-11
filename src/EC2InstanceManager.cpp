#include "EC2InstanceManager.h"

EC2InstanceManager::EC2InstanceManager() {
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);
}

EC2InstanceManager::~EC2InstanceManager() {
    Aws::ShutdownAPI(options);
}

QString EC2InstanceManager::startEC2Instance(const QString& instanceID) const {
    if(!contains(instanceID))
        return "Instance " + instanceID + " doesn't exist!";

    Aws::EC2::Model::StartInstancesOutcome outcome = instances.at(instanceID.toStdString()).start();

    if(!outcome.IsSuccess())
       return "Failed to start instance " + instanceID + ": " + outcome.GetError().GetMessage().c_str();
    else
        return "";
}

QString EC2InstanceManager::stopEC2Instance(const QString& instanceID) const {
    if(!contains(instanceID))
        return "Instance " + instanceID + " doesn't exist!";

    Aws::EC2::Model::StopInstancesOutcome outcome = instances.at(instanceID.toStdString()).stop();

    if(!outcome.IsSuccess())
        return "Failed to stop instance " + instanceID + ": " + outcome.GetError().GetMessage().c_str();
    else
        return "";
}

QString EC2InstanceManager::rebootEC2Instance(const QString& instanceID) const {
    if(!contains(instanceID))
        return "Instance " + instanceID + " doesn't exist!";

    Aws::EC2::Model::RebootInstancesOutcome outcome = instances.at(instanceID.toStdString()).reboot();

    if(!outcome.IsSuccess())
        return "Failed to reboot instance " + instanceID + ": " + outcome.GetError().GetMessage().c_str();
    else
        return "";
}

QString EC2InstanceManager::addEC2Instance(const QString& instanceID) {
    if(contains(instanceID))
        return "Instance " + instanceID + " already exists!";
    else {
        instances[instanceID.toStdString()] = EC2Instance(instanceID.toStdString());
        return "";
    }
}

QString EC2InstanceManager::removeEC2Instance(const QString& instanceID) {
    if(!contains(instanceID))
        return "Instance " + instanceID + " doesn't exist!";
    else {
        instances.erase(instanceID.toStdString());
        return "";
    }
}

std::vector<QString> EC2InstanceManager::listEC2Instances() {
    std::vector<QString> instancesList;

    for(const std::pair<Aws::String, EC2Instance>& instance : instances)
        instancesList.push_back(QString::fromStdString(instance.first));

    return instancesList;
}

bool EC2InstanceManager::contains(const QString& instanceID) const {
    return instances.find(instanceID.toStdString()) != instances.end();
}
