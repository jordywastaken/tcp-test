
#pragma once

#include <sys/ppu_thread.h>

class Thread
{
public:
    union Job
    {
        uint64_t packed;
        struct
        {
            void(*callback)(void*);
            void* userdata;
        };
    };

    Thread() = default;

    Thread(void(*callback)(), const char* name = "", int priority = 1000, int stackSize = 2048, int flag = SYS_PPU_THREAD_CREATE_JOINABLE)
    {
        sys_ppu_thread_create(&id, [](uint64_t arg) -> void
            {
                if (arg)
                    reinterpret_cast<void(*)()>((uint32_t)arg)();

                sys_ppu_thread_exit(0);

            }, reinterpret_cast<uint64_t>(callback), priority, stackSize, flag, name);
    }

    Thread(void(*callback)(void*), void* userdata, const char* name = "", int priority = 1000, int stackSize = 2048, int flag = SYS_PPU_THREAD_CREATE_JOINABLE)
    {
        Job job;
        job.callback = callback;
        job.userdata = userdata;

        sys_ppu_thread_create(&id, [](uint64_t arg) -> void
            {
                Job job;
                job.packed = arg;

                if (job.callback)
                    job.callback(job.userdata);

                sys_ppu_thread_exit(0);

            }, job.packed, priority, stackSize, flag, name);
    }

    bool IsStarted()
    {
        return (id != SYS_PPU_THREAD_ID_INVALID);
    }

    bool Join()
    {
        if (!IsStarted())
            return false;

        uint64_t exitCode;
        return sys_ppu_thread_join(id, &exitCode) == CELL_OK;
    }

    void Rename(const char* name)
    {
        if (IsStarted())
            sys_ppu_thread_rename(id, name);
    }

    sys_ppu_thread_t id = SYS_PPU_THREAD_ID_INVALID;
};