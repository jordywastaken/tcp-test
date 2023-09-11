
#include "ThreadPool.h"
#include "Thread.h"
#include "Imports.h"
#include <sys/timer.h>

namespace ThreadPool
{
    constexpr int s_threadPoolSize = 5;
    constexpr int s_jobQueueSize = 20;

    bool s_running;
    Thread s_threadPool[s_threadPoolSize];
    Thread::Job s_jobQueue[s_jobQueueSize];

    bool IsQueueFull()
    {
        for (int i = 0; i < s_jobQueueSize; i++)
        {
            if (s_jobQueue[i].packed == 0)
                return false;
        }

        return true;
    }

    void WorkingLoop()
    {
        while (s_running)
        {
            if (s_jobQueue[0].packed)
            {
                auto job = s_jobQueue[0].callback;
                auto userdata = s_jobQueue[0].userdata;

                // Pop first entry
                for (int i = 0; i < s_jobQueueSize - 1; i++)
                {
                    s_jobQueue[i] = s_jobQueue[i + 1];
                    s_jobQueue[i + 1].packed = 0;
                }

                job(userdata);
            }

            // Sleep 10 milliseconds
            sys_timer_usleep(10 * 1000);
        }
    }

    void Initialize()
    {
        s_running = true;

        for (int i = 0; i < s_threadPoolSize; i++)
        {
            char threadName[64];
            snprintf(threadName, 64, "WorkerThread%i", i);

            s_threadPool[i] = Thread(WorkingLoop, threadName, 1000, 2 * 1024);
        }
    }

    void Terminate()
    {
        s_running = false;

        for (auto& activeThread : s_threadPool)
            activeThread.Join();
    }

    void QueueJob(void(*job)(void*), void* userdata)
    {
        // Using a mutex would be better...
        while (IsQueueFull())
        {
            // Sleep 10 milliseconds
            sys_timer_usleep(10 * 1000);
        }

        for (int i = 0; i < s_threadPoolSize; i++)
        {
            if (!s_jobQueue[i].packed)
            {
                s_jobQueue[i].callback = job;
                s_jobQueue[i].userdata = userdata;
                return;
            }
        }
    }
}