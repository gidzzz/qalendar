#ifndef CHANGECLIENT_H
#define CHANGECLIENT_H

#include "ChangeManager.h"

class ChangeClient
{
public:
    ChangeClient() :
        version(0)
    {
    }

protected:
    void sync()
    {
        version = ChangeManager::version();
    }

    bool isOutdated()
    {
        return version != ChangeManager::version();
    }


private:
    Version version;
};

#endif // CHANGECLIENT_H
