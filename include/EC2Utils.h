#ifndef EC2UTILS_H
#define EC2UTILS_H

#include <aws/core/Aws.h>
#include <aws/ec2/EC2Client.h>

class EC2Instance {
    public:
        EC2Instance();
        EC2Instance(const Aws::String& instanceID);
        ~EC2Instance();

        Aws::EC2::Model::StartInstancesOutcome start() const;
        Aws::EC2::Model::StopInstancesOutcome stop() const;
        Aws::EC2::Model::RebootInstancesOutcome reboot() const;

        Aws::EC2::Model::DescribeInstancesOutcome describeInstance() const;

        void setInstanceID(const Aws::String& instanceID);
        Aws::String getInstanceID() const;

        void setName(const Aws::String& name);
        Aws::String getName() const;

    private:
        Aws::String instanceID;
        Aws::String name;
};

#endif
