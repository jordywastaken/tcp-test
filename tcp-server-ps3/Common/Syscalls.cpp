
#include "Syscalls.h"

__attribute__((naked, noinline)) uint64_t Syscall(int id, ...)
{
    asm
    (
        ".set STACK_SIZE, 0x70;"
        ".set LINK_REGISTER, 0x10;"

        "mflr %r0;"
        "std %r0, LINK_REGISTER(%r1);"
        "stdu %r1, -STACK_SIZE(%r1);"

        "mr %r11, %r3;"
        "mr %r3, %r4;"
        "mr %r4, %r5;"
        "mr %r5, %r6;"
        "mr %r6, %r7;"
        "mr %r7, %r8;"
        "mr %r8, %r9;"
        "mr %r9, %r10;"
        "sc;"

        "addi %r1, %r1, STACK_SIZE;"
        "ld %r0, LINK_REGISTER(%r1);"
        "mtlr %r0;"
        "blr;"
    );

    return __reg(3);
}

__attribute__((naked, noinline)) uint64_t CobraCall(int id, ...)
{
    asm
    (
        ".set STACK_SIZE, 0x70;"
        ".set LINK_REGISTER, 0x10;"

        "mflr %r0;"
        "std %r0, LINK_REGISTER(%r1);"
        "stdu %r1, -STACK_SIZE(%r1);"

        "li %r11, 8;"
        "sc;"

        "addi %r1, %r1, STACK_SIZE;"
        "ld %r0, LINK_REGISTER(%r1);"
        "mtlr %r0;"
        "blr;"
    );

    return __reg(3);
}

bool DbgGetProcessList(int list[16])
{
    return CobraCall(0x7777, 0x21, list) == 0;
}

bool DbgGetProcessName(int pid, char name[32])
{
    return CobraCall(0x7777, 0x22, pid, name) == 0;
}

bool DbgReadProcessMemory(int pid, int address, void* data, int size)
{
    return CobraCall(0x7777, 0x31, pid, address, data, size) == 0;
}

bool DbgWriteProcessMemory(int pid, int address, const void* data, int size)
{
    return CobraCall(0x7777, 0x32, pid, address, data, size) == 0;
}

bool DbgAllocateProcessPage(int pid, int size, int pageSize, int flags, bool isExecutable, uint64_t* pageTable)
{
    return CobraCall(0x7777, 0x33, pid, size, pageSize, flags, isExecutable, pageTable);
}

bool DbgFreeProcessPage(int pid, int flags, uint64_t* pageTable)
{
    return CobraCall(0x7777, 0x34, pid, flags, pageTable);
}