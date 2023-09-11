
#pragma once

// sysPrxForUser imports
extern "C"
{
    int   _sys_printf(const char* fmt, ...);
    int   _sys_snprintf(char* buffer, int size, const char* fmt, ...);
    int   _sys_strncmp(const char* str1, const char* str2, int n);
    int   _sys_strlen(const char* str);
    void* _sys_memset(void* ptr, int c, int size);

    void* _sys_malloc(int size);
    void _sys_free(void* ptr);
}

#define printf                _sys_printf
#define snprintf              _sys_snprintf
#define strncmp               _sys_strncmp
#define strlen                _sys_strlen

#define ZeroMemory(ptr, size)       _sys_memset(ptr, 0, size)
#define DebugPrint(from, fmt, ...)  _sys_printf("[" from "]: " fmt, ##__VA_ARGS__)