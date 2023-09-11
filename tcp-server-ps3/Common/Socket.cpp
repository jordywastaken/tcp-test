
#include "Socket.h"

namespace Socket
{
    int SetupServer(int port, int backlog)
    {
        int s = socket(AF_INET, SOCK_STREAM, 0);

        timeval transmissionTimeOut = { 2, 0 };
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &transmissionTimeOut, sizeof(timeval));
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &transmissionTimeOut, sizeof(timeval));

        int reusePort = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(int));

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = port;

        if (bind(s, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in)) < 0)
        {
            Close(s);
            return -1;
        }

        if (listen(s, backlog) < 0)
        {
            Close(s);
            return -1;
        }

        return s;
    }

    int Stat(int s)
    {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(s, &set);

        timeval timeOut = { 2, 0 };
        return socketselect(s + 1, &set, 0, 0, &timeOut);
    }

    int AcceptNewConnection(int s, sockaddr_in* address)
    {
        sockaddr_in backup;
        socklen_t len = sizeof(sockaddr_in);

        return accept(s, reinterpret_cast<sockaddr*>(address ? address : &backup), &len);
    }

    void Close(int s)
    {
        socketclose(s);
    }

    bool SendInt(int s, int value)
    {
        return send(s, &value, sizeof(int), 0) > 0;
    }

    bool RecvInt(int s, int* value)
    {
        return recv(s, value, sizeof(int), 0) > 0;
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