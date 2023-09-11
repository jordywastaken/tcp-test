
#include "Socket.h"
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

bool InitializeWinsock()
{
    WSADATA wsData = { 0 };
    unsigned short wsVersion = MAKEWORD(2, 2);

    int wsOk = WSAStartup(wsVersion, &wsData);
    if (wsOk < 0)
    {
        printf("[Winsock]: Failed to startup : %X\n", wsOk);
        return false;
    }

    return true;
}

void TerminateWinsock()
{
    WSACleanup();
}

namespace Socket
{
    int Connect(const char* ip, int port)
    {
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0)
        {
            printf("[Socket]: Failed to create : %X\n", WSAGetLastError());
            return s;
        }

        sockaddr_in serverAddress;
        ZeroMemory(&serverAddress, sizeof(serverAddress));

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        inet_pton(AF_INET, ip, &serverAddress.sin_addr);

        int connectionStatus = connect(s, (sockaddr*)&serverAddress, sizeof(serverAddress));
        if (connectionStatus < 0)
        {
            printf("[Client]: Error connecting to %s:%i : %X\n", ip, port, WSAGetLastError());
            closesocket(s);
            return connectionStatus;
        }

        return s;
    }

    bool SendInt(int s, int value)
    {
        return send(s, (const char*)&value, sizeof(int), 0) > 0;
    }

    bool RecvInt(int s, int* value)
    {
        return recv(s, (char*)value, sizeof(int), 0) > 0;
    }

    bool SendLargeData(int s, const void* data, size_t len)
    {
        char* pos = (char*)(data);

        while (len)
        {
            size_t chunkSize = len > 8192 ? 8192 : len;
            int bytesSent = send(s, pos, chunkSize, 0);

            if (bytesSent < 0)
                return false;

            len -= bytesSent;
            pos += bytesSent;
        }

        return true;
    }

    bool RecvLargeData(int s, void* data, size_t len)
    {
        char* pos = (char*)(data);

        while (len)
        {
            size_t chunkSize = len > 8192 ? 8192 : len;
            int bytesReceived = recv(s, pos, chunkSize, 0);

            if (bytesReceived < 0)
                return false;

            len -= bytesReceived;
            pos += bytesReceived;
        }

        return true;
    }
}