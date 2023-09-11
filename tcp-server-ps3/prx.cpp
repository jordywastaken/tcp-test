
#include <sys/prx.h>

#include "Common/Imports.h"
#include "Common/ThreadPool.h"
#include "Server/API.h"

#pragma diag_suppress 77
SYS_MODULE_INFO(tcp-server-ps3, 0, 1, 1);
SYS_MODULE_START(module_start);
SYS_MODULE_STOP(module_stop);

extern "C"
{
    int module_start(size_t args, void *argv)
    {
        ThreadPool::Initialize();
        API::Initialize();
        return 0;
    }

    int module_stop(size_t args, void *argv)
    {
        API::Terminate();
        ThreadPool::Terminate();
        return 0;
    }
}