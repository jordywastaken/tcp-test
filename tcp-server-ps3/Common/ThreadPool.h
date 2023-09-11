
#pragma once

namespace ThreadPool
{
    void Initialize();
    void Terminate();

    void QueueJob(void(*job)(void*), void* userdata);
}