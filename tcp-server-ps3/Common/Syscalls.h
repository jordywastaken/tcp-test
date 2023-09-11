
#pragma once

#include <stdint.h>

uint64_t Syscall(int id, ...);

bool DbgGetProcessList(int list[16]);
bool DbgGetProcessName(int pid, char name[32]);
bool DbgReadProcessMemory(int pid, int address, void* data, int size);
bool DbgWriteProcessMemory(int pid, int address, const void* data, int size);
bool DbgAllocateProcessPage(int pid, int size, int pageSize, int flags, bool isExecutable, uint64_t* pageTable);
bool DbgFreeProcessPage(int pid, int flags, uint64_t* pageTable);
