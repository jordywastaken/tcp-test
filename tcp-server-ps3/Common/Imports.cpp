
#include "Imports.h"
#include <stdint.h>

extern "C"
{
    void* memset(void* ptr, int c, int size)
    {
        return _sys_memset(ptr, c, size);
    }
}

void* operator new(size_t size)
{
    return _sys_malloc(size);
}

void* operator new[](size_t size)
{
    return _sys_malloc(size);
}

void operator delete(void* ptr)
{
    _sys_free(ptr);
}

void operator delete[](void* ptr)
{
    _sys_free(ptr);
}