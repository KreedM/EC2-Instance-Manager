#include "EC2InstanceManager.h"

EC2InstanceManager::EC2InstanceManager() {
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);
}

EC2InstanceManager::~EC2InstanceManager() {
    Aws::ShutdownAPI(options);
}

bool EC2InstanceManager::startEC2Instance(const QString& instanceID) const {
    return false;
}

bool EC2InstanceManager::stopEC2Instance(const QString& instanceID) const {
    return false;
}

bool EC2InstanceManager::rebootEC2Instance(const QString& instanceID) const {
    return false;
}
