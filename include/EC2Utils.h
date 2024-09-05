#ifndef EC2UTILS_H
#define EC2UTILS_H

#include <aws/core/Aws.h>

class EC2Instance {
    public:
        EC2Instance();
        EC2Instance(char *instanceID);
        ~EC2Instance();

        bool start();
        bool stop();
        bool reboot();

    private:
        Aws::String instanceID;
};

#endif