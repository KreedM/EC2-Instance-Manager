#include "EC2Utils.h"
#include <iostream>
#include <aws/ec2/EC2Client.h>
#include <aws/ec2/model/StartInstancesRequest.h>
#include <aws/ec2/model/StopInstancesRequest.h>
#include <aws/ec2/model/RebootInstancesRequest.h>

EC2Instance::EC2Instance() {

}

EC2Instance::EC2Instance(char *instanceID) : EC2Instance() {
    this->instanceID = instanceID;
}

EC2Instance::~EC2Instance() {
    
}

bool EC2Instance::start() {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::EC2::EC2Client ec2Client(clientConfig);

    Aws::EC2::Model::StartInstancesRequest startRequest;
    startRequest.AddInstanceIds(instanceID);
    
    // Dry run
    startRequest.SetDryRun(true);
    Aws::EC2::Model::StartInstancesOutcome dryRunOutcome = ec2Client.StartInstances(startRequest);
    
    if (dryRunOutcome.IsSuccess()) { 
        std::cerr << "Failed dry run to start instance. A dry run should trigger an error." << std::endl;

        return false;
    } 
    else if (dryRunOutcome.GetError().GetErrorType() != Aws::EC2::EC2Errors::DRY_RUN_OPERATION) {
        std::cout << "Failed dry run to start instance " << instanceID << ": " << dryRunOutcome.GetError().GetMessage() << std::endl;

        return false;
    }

    // Actual run
    startRequest.SetDryRun(false);
    Aws::EC2::Model::StartInstancesOutcome startInstanceOutcome = ec2Client.StartInstances(startRequest);

    if (!startInstanceOutcome.IsSuccess()) {
        std::cout << "Failed to start instance " << instanceID << ": " << startInstanceOutcome.GetError().GetMessage() << std::endl;
        
        return false;
    } 
    else {
        std::cout << "Successfully started instance " << instanceID << std::endl;

        return true;
    }
}

bool EC2Instance::stop() {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::EC2::EC2Client ec2Client(clientConfig);

    Aws::EC2::Model::StopInstancesRequest stopRequest;
    stopRequest.AddInstanceIds(instanceID);
    
    // Dry run
    stopRequest.SetDryRun(true);
    Aws::EC2::Model::StopInstancesOutcome dryRunOutcome = ec2Client.StopInstances(stopRequest);

    if (dryRunOutcome.IsSuccess()) { 
        std::cerr << "Failed dry run to stop instance. A dry run should trigger an error." << std::endl;

        return false;
    } 
    else if (dryRunOutcome.GetError().GetErrorType() != Aws::EC2::EC2Errors::DRY_RUN_OPERATION) {
        std::cout << "Failed dry run to stop instance " << instanceID << ": " << dryRunOutcome.GetError().GetMessage() << std::endl;

        return false;
    }

    // Actual run
    stopRequest.SetDryRun(false);
    Aws::EC2::Model::StopInstancesOutcome stopInstanceOutcome = ec2Client.StopInstances(stopRequest);

    if (!stopInstanceOutcome.IsSuccess()) {
        std::cout << "Failed to stop instance " << instanceID << ": " << stopInstanceOutcome.GetError().GetMessage() << std::endl;

        return false;
    } 
    else {
        std::cout << "Successfully stopped instance " << instanceID << std::endl;
        
        return true;
    }
}

bool EC2Instance::reboot() {
    Aws::Client::ClientConfiguration clientConfig;
    Aws::EC2::EC2Client ec2Client(clientConfig);

    Aws::EC2::Model::RebootInstancesRequest request;
    request.AddInstanceIds(instanceID);

    // Dry run
    request.SetDryRun(true);
    Aws::EC2::Model::RebootInstancesOutcome dryRunOutcome = ec2Client.RebootInstances(request);

    if (dryRunOutcome.IsSuccess()) {
        std::cerr << "Failed dry run to reboot on instance. A dry run should trigger an error." << std::endl;
        
        return false;
    } 
    else if (dryRunOutcome.GetError().GetErrorType() != Aws::EC2::EC2Errors::DRY_RUN_OPERATION) {
        std::cout << "Failed dry run to reboot instance " << instanceID << ": " << dryRunOutcome.GetError().GetMessage() << std::endl;
        
        return false;
    }

    // Actual run
    request.SetDryRun(false);
    Aws::EC2::Model::RebootInstancesOutcome outcome = ec2Client.RebootInstances(request);

    if (!outcome.IsSuccess()) {
        std::cout << "Failed to reboot instance " << instanceID << ": " << outcome.GetError().GetMessage() << std::endl;
        
        return false;
    } 
    else {
        std::cout << "Successfully rebooted instance " << instanceID << std::endl;

        return true;
    }
}