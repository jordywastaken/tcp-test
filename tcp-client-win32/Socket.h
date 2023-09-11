
#pragma once

#include <WS2tcpip.h>

template <typename T>
static inline T SwapEndian(T u)
{
    union {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (int k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

bool InitializeWinsock();
void TerminateWinsock();

namespace Socket
{
    int Connect(const char* ip, int port);

    bool SendInt(int s, int value);
    bool RecvInt(int s, int* value);

    bool SendLargeData(int s, const void* data, size_t len);
    bool RecvLargeData(int s, void* data, size_t len);

    template<class T>
    static T* ReceiveType(int s)
    {
        T* packet = new T();

        if (RecvLargeData(s, packet, sizeof(T)))
            return packet;

        delete packet;
        return nullptr;
    }
}