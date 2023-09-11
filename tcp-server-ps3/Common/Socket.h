
#pragma once

#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

namespace Socket
{
    int SetupServer(int port, int backlog);
    int Stat(int s);
    int AcceptNewConnection(int s, sockaddr_in* address);
    void Close(int s);

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