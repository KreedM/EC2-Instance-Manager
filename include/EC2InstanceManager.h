#ifndef EC2INSTANCEMANAGER_H
#define EC2INSTANCEMANAGER_H

#include "EC2Utils.h"
#include <unordered_set>
#include <QString>

class EC2InstanceManager {
    public:
        EC2InstanceManager();
        ~EC2InstanceManager();

        bool startEC2Instance(const QString& instanceID) const;
        bool stopEC2Instance(const QString& instanceID) const;
        bool rebootEC2Instance(const QString& instanceID) const;

    private:
        std::unordered_set<Aws::String, EC2Instance> instances;
        Aws::SDKOptions options;
};

#endif
