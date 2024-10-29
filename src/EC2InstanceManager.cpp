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

QString EC2InstanceManager::getEC2InstanceStatus(const QString& instanceID) const {
    if(!contains(instanceID))
        return "";

    return QString::fromStdString(instances.at(instanceID.toStdString()).getState());
}

QString EC2InstanceManager::describeEC2Instance(const QString& instanceID, std::vector<std::pair<QString, QString>>& descriptions, bool instanceSummary, bool instanceDetails, bool hostAndPlacementGroup, bool capacityReservation) {
    if(!contains(instanceID))
        return "Instance " + instanceID + " doesn't exist!";

    const Aws::EC2::Model::DescribeInstancesOutcome& outcome = instances.at(instanceID.toStdString()).describeInstance();

    if(!outcome.IsSuccess())
        return "Failed to describe instance " + instanceID + ": " + outcome.GetError().GetMessage().c_str();

    const Aws::EC2::Model::Instance& instance = outcome.GetResult().GetReservations().front().GetInstances().front();

    QString tagsStr = "", name = "";
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

    instances.at(instanceID.toStdString()).setState(Aws::EC2::Model::InstanceStateNameMapper::GetNameForInstanceStateName(instance.GetState().GetName()));

    if(instanceSummary)
        describeEC2InstanceSummary(instance, descriptions);

    if(instanceDetails)
        describeEC2InstanceDetails(instance, descriptions);

    if(hostAndPlacementGroup)
        describeEC2InstanceHostAndPlacementGroup(instance, descriptions, outcome.GetResult().GetReservations().front().GetReservationId());

    if(capacityReservation)
        describeEC2InstanceCapacityReservation(instance, descriptions);

    return "";
}

void EC2InstanceManager::describeEC2InstanceSummary(const Aws::EC2::Model::Instance& instance, std::vector<std::pair<QString, QString>>& descriptions) {
    descriptions.push_back(
        {
            "Instance ID",
            QString::fromStdString(instance.GetInstanceId())
        }
    );

    descriptions.push_back(
        {
            "Public IPv4 Address",
            QString::fromStdString(instance.GetPublicIpAddress())
        }
    );

    descriptions.push_back(
        {
            "Private IPv4 Address",
            QString::fromStdString(instance.GetPrivateIpAddress())
        }
    );

    descriptions.push_back(
        {
            "IPv6 Address",
            QString::fromStdString(instance.GetIpv6Address())
        }
    );

    descriptions.push_back(
        {
            "Instance State",
            QString::fromStdString(Aws::EC2::Model::InstanceStateNameMapper::GetNameForInstanceStateName(instance.GetState().GetName()))
        }
    );

    descriptions.push_back(
        {
            "Public IPv4 DNS",
            QString::fromStdString(instance.GetPublicDnsName())
        }
    );

    Aws::EC2::Model::PrivateDnsNameOptionsResponse optionsResponse = instance.GetPrivateDnsNameOptions();

    QString hostnameType;
    switch(optionsResponse.GetHostnameType()) {
        case Aws::EC2::Model::HostnameType::NOT_SET: hostnameType = "Not set"; break;
        case Aws::EC2::Model::HostnameType::ip_name: hostnameType = "IP name"; break;
        case Aws::EC2::Model::HostnameType::resource_name: hostnameType = "Resource name";
    }

    descriptions.push_back(
            {
                "Hostname Type",
                hostnameType
            }
    );

    descriptions.push_back(
        {
            "Private IP DNS Name (IPv4 Only)",
            QString::fromStdString(instance.GetPrivateDnsName())
        }
    );

    QString answerPrivateDNSName;
    if(optionsResponse.GetEnableResourceNameDnsARecord() && optionsResponse.GetEnableResourceNameDnsAAAARecord())
        answerPrivateDNSName = "IPv4 and IPv6 (A and AAAA)";
    else if(optionsResponse.GetEnableResourceNameDnsARecord())
        answerPrivateDNSName = "IPv4 (A)";
    else if(optionsResponse.GetEnableResourceNameDnsAAAARecord())
        answerPrivateDNSName = "IPv6 (AAAA)";

    descriptions.push_back(
        {
            "Answer Private Resource DNS Name",
            answerPrivateDNSName
        }
    );

    descriptions.push_back(
        {
            "Instance Type",
            QString::fromStdString(Aws::EC2::Model::InstanceTypeMapper::GetNameForInstanceType(instance.GetInstanceType()))
        }
    );

    descriptions.push_back(
        {
            "VPC ID",
         QString::fromStdString(instance.GetVpcId())
        }
    );

    descriptions.push_back(
        {
            "IAM Role",
            QString::fromStdString(instance.GetIamInstanceProfile().GetArn())
        }
    );

    descriptions.push_back(
        {
            "Subnet ID",
            QString::fromStdString(instance.GetSubnetId())
        }
    );

    QString IMDSv2Required;
    if(instance.GetMetadataOptions().GetHttpTokens() == Aws::EC2::Model::HttpTokensState::required)
        IMDSv2Required = "Required";
    else if(instance.GetMetadataOptions().GetHttpTokens() == Aws::EC2::Model::HttpTokensState::optional)
        IMDSv2Required = "Optional";

    descriptions.push_back(
        {
            "IMDSv2",
            IMDSv2Required
        }
    );

    descriptions.push_back(
        {
            "Instance ARN",
            QString::fromStdString("arn:aws:ec2:*:*:instance/" + instance.GetInstanceId())
        }
    );
}

void EC2InstanceManager::describeEC2InstanceDetails(const Aws::EC2::Model::Instance& instance, std::vector<std::pair<QString, QString>>& descriptions) {
    descriptions.push_back(
        {
            "AMI ID",
            QString::fromStdString(instance.GetImageId())
        }
    );

    descriptions.push_back(
        {
            "Monitoring",
            QString::fromStdString(Aws::EC2::Model::MonitoringStateMapper::GetNameForMonitoringState(instance.GetMonitoring().GetState()))
        }
    );

    descriptions.push_back(
        {
            "Platform Details",
            QString::fromStdString(instance.GetPlatformDetails())
        }
    );

    descriptions.push_back(
        {
            "Launch Time",
            QString::fromStdString(instance.GetLaunchTime().ToLocalTimeString(Aws::Utils::DateFormat::RFC822))
        }
    );

    descriptions.push_back(
        {
            "Instance Auto-Recovery",
            QString::fromStdString(Aws::EC2::Model::InstanceAutoRecoveryStateMapper::GetNameForInstanceAutoRecoveryState(instance.GetMaintenanceOptions().GetAutoRecovery()))
        }
    );

    QString lifecycle = QString::fromStdString(Aws::EC2::Model::InstanceLifecycleTypeMapper::GetNameForInstanceLifecycleType(instance.GetInstanceLifecycle()));
    if(lifecycle.isEmpty())
        lifecycle = "normal";

    descriptions.push_back(
        {
            "Lifecycle",
            lifecycle
        }
    );

    descriptions.push_back(
        {
            "Stop-Hibernate Behavior",
            instance.GetHibernationOptions().GetConfigured() ? "Enabled" : "Disabled"
        }
    );

    descriptions.push_back(
        {
            "AMI Launch Index",
            QString::number(instance.GetAmiLaunchIndex())
        }
    );

    descriptions.push_back(
        {
            "Key Pair Assigned at Launch",
            QString::fromStdString(instance.GetKeyName())
        }
    );

    descriptions.push_back(
        {
            "State Transition Reason",
                QString::fromStdString(instance.GetStateTransitionReason())
        }
    );

    descriptions.push_back(
        {
            "Kernel ID",
            QString::fromStdString(instance.GetKernelId())
        }
    );

    descriptions.push_back(
        {
            "State Transition Message",
            QString::fromStdString(instance.GetStateReason().GetMessage())
        }
    );

    descriptions.push_back(
        {
            "Usage Operation",
            QString::fromStdString(instance.GetUsageOperation())
        }
    );

    descriptions.push_back(
        {
            "RAM Disk ID",
            QString::fromStdString(instance.GetRamdiskId())
        }
    );

    descriptions.push_back(
        {
            "Enclaves Support",
            instance.GetEnclaveOptions().GetEnabled() ? "Enabled" : "Disabled"
        }
    );

    descriptions.push_back(
        {
            "Boot Mode",
            QString::fromStdString(Aws::EC2::Model::BootModeValuesMapper::GetNameForBootModeValues(instance.GetBootMode()))
        }
    );

    descriptions.push_back(
        {
            "Current Instance Boot Mode",
            QString::fromStdString(Aws::EC2::Model::InstanceBootModeValuesMapper::GetNameForInstanceBootModeValues(instance.GetCurrentInstanceBootMode()))
        }
    );

    descriptions.push_back(
        {
            "Allow Tags in Instance Metadata",
            QString::fromStdString(Aws::EC2::Model::InstanceMetadataTagsStateMapper::GetNameForInstanceMetadataTagsState(instance.GetMetadataOptions().GetInstanceMetadataTags()))
        }
    );
}

void EC2InstanceManager::describeEC2InstanceHostAndPlacementGroup(const Aws::EC2::Model::Instance& instance, std::vector<std::pair<QString, QString>>& descriptions, const Aws::String& reservationID) {
    Aws::EC2::Model::Placement placement = instance.GetPlacement();

    descriptions.push_back(
        {
            "Host ID",
            QString::fromStdString(placement.GetHostId())
        }
    );

    descriptions.push_back(
        {
            "Affinity",
            QString::fromStdString(placement.GetAffinity())
        }
    );

    descriptions.push_back(
        {
            "Placement Group",
            QString::fromStdString(placement.GetGroupName())
        }
    );

    descriptions.push_back(
        {
            "Host Resource Group Name",
            QString::fromStdString(placement.GetHostResourceGroupArn())
        }
    );

    descriptions.push_back(
        {
            "Tenancy",
            QString::fromStdString(Aws::EC2::Model::TenancyMapper::GetNameForTenancy(placement.GetTenancy()))
        }
    );

    descriptions.push_back(
        {
            "Placement Group ID",
            QString::fromStdString(placement.GetGroupId())
        }
    );

    descriptions.push_back(
        {
            "Virtualization Type",
            QString::fromStdString(Aws::EC2::Model::VirtualizationTypeMapper::GetNameForVirtualizationType(instance.GetVirtualizationType()))
        }
    );

    descriptions.push_back(
        {
            "Reservation",
            QString::fromStdString(reservationID)
        }
    );

    descriptions.push_back(
        {
            "Partition Number",
            QString::number(placement.GetPartitionNumber())
        }
    );

    descriptions.push_back(
        {
            "Number of vCPUs",
            QString::number(instance.GetCpuOptions().GetCoreCount())
        }
    );
}

void EC2InstanceManager::describeEC2InstanceCapacityReservation(const Aws::EC2::Model::Instance& instance, std::vector<std::pair<QString, QString>>& descriptions) {
    descriptions.push_back(
        {
            "Capacity Reservation ID",
            QString::fromStdString(instance.GetCapacityReservationId())
        }
    );

    descriptions.push_back(
        {
            "Capacity Reservation Setting",
            QString::fromStdString(Aws::EC2::Model::CapacityReservationPreferenceMapper::GetNameForCapacityReservationPreference(instance.GetCapacityReservationSpecification().GetCapacityReservationPreference()))
        }
    );
}
