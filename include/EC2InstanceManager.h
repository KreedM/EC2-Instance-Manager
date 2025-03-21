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
        std::vector<std::pair<QString, QString>> saveEC2Instances();
        void clearEC2Instances();

        QString getEC2InstanceName(const QString& instanceID) const;
        void setEC2InstanceName(const QString& instanceID, const QString& name);
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
