#include "EC2Utils.h"
#include <aws/ec2/model/RebootInstancesRequest.h>
#include <aws/ec2/model/StartInstancesRequest.h>
#include <aws/ec2/model/StopInstancesRequest.h>

EC2Instance::EC2Instance() {
    name = "";
}

EC2Instance::EC2Instance(const Aws::String& instanceID) : EC2Instance() {
    this->instanceID = instanceID;
}

EC2Instance::~EC2Instance() {
    
}

Aws::EC2::Model::StartInstancesOutcome EC2Instance::start() const {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::EC2::EC2Client ec2Client(clientConfig);

    Aws::EC2::Model::StartInstancesRequest startRequest;
    startRequest.AddInstanceIds(instanceID);
    return ec2Client.StartInstances(startRequest);
}

Aws::EC2::Model::StopInstancesOutcome EC2Instance::stop() const {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::EC2::EC2Client ec2Client(clientConfig);

    Aws::EC2::Model::StopInstancesRequest stopRequest;
    stopRequest.AddInstanceIds(instanceID);
    return ec2Client.StopInstances(stopRequest);
}

Aws::EC2::Model::RebootInstancesOutcome EC2Instance::reboot() const {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::EC2::EC2Client ec2Client(clientConfig);

    Aws::EC2::Model::RebootInstancesRequest rebootRequest;
    rebootRequest.AddInstanceIds(instanceID);
    return ec2Client.RebootInstances(rebootRequest);
}

Aws::EC2::Model::DescribeInstancesOutcome EC2Instance::describeInstance() const {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::EC2::EC2Client ec2Client(clientConfig);

    Aws::EC2::Model::DescribeInstancesRequest describeInstancesRequest;
    describeInstancesRequest.AddInstanceIds(instanceID);

    return ec2Client.DescribeInstances(describeInstancesRequest);
}

void EC2Instance::setInstanceID(const Aws::String& instanceID) {
    this->instanceID = instanceID;
}

Aws::String EC2Instance::getInstanceID() const {
    return instanceID;
}

void EC2Instance::setName(const Aws::String& name) {
    this->name = name;
}

Aws::String EC2Instance::getName() const {
    return name;
}

void EC2Instance::setState(const Aws::String& state) {
    this->state = state;
}

Aws::String EC2Instance::getState() const {
    return state;
}
