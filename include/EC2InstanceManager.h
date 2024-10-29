#ifndef EC2INSTANCEMANAGER_H
#define EC2INSTANCEMANAGER_H

#include "EC2Utils.h"
#include <unordered_map>
#include <QString>

class EC2InstanceManager {
    public:
        EC2InstanceManager();
        ~EC2InstanceManager();

        QString startEC2Instance(const QString& instanceID) const;
        QString stopEC2Instance(const QString& instanceID) const;
        QString rebootEC2Instance(const QString& instanceID) const;

        QString describeEC2Instance(const QString& instanceID, std::vector<std::pair<QString, QString>>& descriptions, bool instanceSummary, bool instanceDetails, bool hostAndPlacementGroup, bool capacityReservation);

        QString addEC2Instance(const QString& instanceID);
        QString removeEC2Instance(const QString& instanceID);

        std::vector<QString> listEC2Instances();

        QString getEC2InstanceName(const QString& instanceID) const;
        QString getEC2InstanceStatus(const QString& instanceID) const;

        bool contains(const QString& instanceID) const;

    private:
        void describeEC2InstanceSummary(const Aws::EC2::Model::Instance& instance, std::vector<std::pair<QString, QString>>& descriptions);
        void describeEC2InstanceDetails(const Aws::EC2::Model::Instance& instance, std::vector<std::pair<QString, QString>>& descriptions);
        void describeEC2InstanceHostAndPlacementGroup(const Aws::EC2::Model::Instance& instance, std::vector<std::pair<QString, QString>>& descriptions, const Aws::String& reservationID);
        void describeEC2InstanceCapacityReservation(const Aws::EC2::Model::Instance& instance, std::vector<std::pair<QString, QString>>& descriptions);

        std::unordered_map<Aws::String, EC2Instance> instances;
        Aws::SDKOptions options;
};

#endif
