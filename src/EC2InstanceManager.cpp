#include "EC2InstanceManager.h"
#include <qdebug.h>

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

    const Aws::EC2::Model::StartInstancesOutcome& outcome = instances.at(instanceID.toStdString()).start();

    if(!outcome.IsSuccess())
       return "Failed to start instance " + instanceID + ": " + QString::fromStdString(outcome.GetError().GetMessage());
    else
        return "";
}

QString EC2InstanceManager::stopEC2Instance(const QString& instanceID) const {
    if(!contains(instanceID))
        return "Instance " + instanceID + " doesn't exist!";

    const Aws::EC2::Model::StopInstancesOutcome& outcome = instances.at(instanceID.toStdString()).stop();

    if(!outcome.IsSuccess())
        return "Failed to stop instance " + instanceID + ": " + QString::fromStdString(outcome.GetError().GetMessage());
    else
        return "";
}

QString EC2InstanceManager::rebootEC2Instance(const QString& instanceID) const {
    if(!contains(instanceID))
        return "Instance " + instanceID + " doesn't exist!";

    const Aws::EC2::Model::RebootInstancesOutcome& outcome = instances.at(instanceID.toStdString()).reboot();

    if(!outcome.IsSuccess())
        return "Failed to reboot instance " + instanceID + ": " + QString::fromStdString(outcome.GetError().GetMessage());
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

    for(const std::pair<Aws::String, EC2Instance>& instance : instances) {
        if(instance.second.getName().empty())
            instancesList.push_back(QString::fromStdString(instance.first));
        else
            instancesList.push_back(QString::fromStdString(instance.first) + " (" + QString::fromStdString(instance.second.getName()) + ")");
    }


    return instancesList;
}

bool EC2InstanceManager::contains(const QString& instanceID) const {
    return instances.find(instanceID.toStdString()) != instances.end();
}


QString EC2InstanceManager::getEC2InstanceName(const QString& instanceID) const {
    if(!contains(instanceID))
        return "";

    return QString::fromStdString(instances.at(instanceID.toStdString()).getName());
}

QString EC2InstanceManager::describeEC2Instance(const QString& instanceID, std::vector<std::pair<QString, QString>>& descriptions) {
    if(!contains(instanceID))
        return "Instance " + instanceID + " doesn't exist!";

    const Aws::EC2::Model::DescribeInstancesOutcome& outcome = instances.at(instanceID.toStdString()).describeInstance();

    if(!outcome.IsSuccess())
        return "Failed to describe instance " + instanceID + ": " + outcome.GetError().GetMessage().c_str();

    const Aws::EC2::Model::Instance& instance = outcome.GetResult().GetReservations().front().GetInstances().front();

    QString name = "", tagsStr = "";
    const std::vector<Aws::EC2::Model::Tag>& tags = instance.GetTags();
    if(!tags.empty()) {
        if(tags[0].GetKey() == "Name")
            name = QString::fromStdString(tags[0].GetValue());

        tagsStr += tags[0].GetKey() + ":" + tags[0].GetValue();

        for(int i = 1; i < tags.size(); ++i) {
            if(name.isEmpty() && tags[i].GetKey() == "Name")
                name = QString::fromStdString(tags[i].GetValue());

            tagsStr += ", " + tags[i].GetKey() + ":" + tags[i].GetValue();
        }
    }

    instances.at(instanceID.toStdString()).setName(name.toStdString());

    descriptions.push_back(
        {
            "Name",
            name
        }
    );

    descriptions.push_back(
        {
            "Tags",
            tagsStr
        }
    );

    descriptions.push_back(
        {
            "ID",
            QString::fromStdString(instance.GetInstanceId())
        }
    );

    descriptions.push_back(
        {
            "AMI",
            QString::fromStdString(instance.GetImageId())
        }
    );

    descriptions.push_back(
        {
            "Type",
            QString::fromStdString(Aws::EC2::Model::InstanceTypeMapper::GetNameForInstanceType(instance.GetInstanceType()))
        }
    );

    descriptions.push_back(
        {
            "State",
            QString::fromStdString(Aws::EC2::Model::InstanceStateNameMapper::GetNameForInstanceStateName(instance.GetState().GetName()))
        }
    );

    descriptions.push_back(
        {
            "Monitoring",
            QString::fromStdString(Aws::EC2::Model::MonitoringStateMapper::GetNameForMonitoringState(instance.GetMonitoring().GetState()))
        }
    );

    return "";
}
