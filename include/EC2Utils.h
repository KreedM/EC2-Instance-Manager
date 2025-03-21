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

        void setState(const Aws::String& state);
        Aws::String getState() const;

    private:
        Aws::String instanceID;
        Aws::String name;
        Aws::String state;
};

#endif
