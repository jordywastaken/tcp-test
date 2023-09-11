
#include "API.h"
#include "Socket.h"
#include <stdio.h>

namespace API
{
    bool SendInitialPacket(int s, int cmd)
    {
        InitialPacket initialPacket;
        initialPacket.version = SwapEndian<int>(API_VERSION);
        initialPacket.cmd = SwapEndian<int>(cmd);
        strncpy_s(initialPacket.magic, PacketMagic, 8);

        if (!Socket::SendLargeData(s, &initialPacket, sizeof(InitialPacket)))
            return false;

        int result = 0;
        if (!Socket::RecvInt(s, &result))
            return false;

        return result == SwapEndian<int>(API_OK);
    }

    bool TestConnectivity(const char* ip)
    {
        int s = Socket::Connect(ip, API_PORT);
        if (!SendInitialPacket(s, API_CMD_TEST_CONNECTIVITY))
            return closesocket(s), false;

        int result = 0;
        if (!Socket::RecvInt(s, &result))
            return closesocket(s), false;

        return closesocket(s), result == SwapEndian<int>(1337);
    }

    bool Notify(const char* ip, const char* icon, const char* text)
    {
        int s = Socket::Connect(ip, API_PORT);
        if (!SendInitialPacket(s, API_CMD_NOTIFY))
            return closesocket(s), false;

        NotifyPacket packet;
        strncpy_s(packet.icon, icon, 32);
        strncpy_s(packet.text, text, 512);
        Socket::SendLargeData(s, &packet, sizeof(NotifyPacket));
        return closesocket(s), true;
    }

    bool DbgGetProcessList(const char* ip, int list[16])
    {
        int s = Socket::Connect(ip, API_PORT);
        if (!SendInitialPacket(s, API_CMD_DBG_GET_PROCESS_LIST))
            return closesocket(s), false;

        DebugProcessListPacket packet;
        if (!Socket::RecvLargeData(s, &packet, sizeof(DebugProcessListPacket)))
            return closesocket(s), false;

        for (int i = 0; i < 16; i++)
            list[i] = SwapEndian<int>(packet.pid[i]);

        return closesocket(s), true;
    }

    bool DbgGetProcessName(const char* ip, int pid, char name[32])
    {
        if (pid == 0)
            return false;

        int s = Socket::Connect(ip, API_PORT);
        if (!SendInitialPacket(s, API_CMD_DBG_GET_PROCESS_NAME))
            return closesocket(s), false;

        Socket::SendInt(s, SwapEndian<int>(pid));
        Socket::RecvLargeData(s, name, 32);
        return closesocket(s), true;
    }

    bool DbgReadProcessMemory(const char* ip, int pid, int address, void* data, int size)
    {
        if (pid == 0 || address == 0 || data == 0 || size == 0)
            return false;

        int s = Socket::Connect(ip, API_PORT);
        if(!SendInitialPacket(s, API_CMD_DBG_READ_PROCESS_MEMORY))
            return closesocket(s), false;

        DebugReadWriteProcessMemoryPacket packet;
        packet.pid = SwapEndian<int>(pid);
        packet.address = SwapEndian<int>(address);
        packet.size = SwapEndian<int>(size);
        Socket::SendLargeData(s, &packet, sizeof(DebugReadWriteProcessMemoryPacket));
        Socket::RecvLargeData(s, data, size);
        return closesocket(s), true;
    }

    bool DbgWriteProcessMemory(const char* ip, int pid, int address, const void* data, int size)
    {
        if (pid == 0 || address == 0 || data == 0 || size == 0)
            return false;

        int s = Socket::Connect(ip, API_PORT);
        if (!SendInitialPacket(s, API_CMD_DBG_WRITE_PROCESS_MEMORY))
            return closesocket(s), false;

        DebugReadWriteProcessMemoryPacket packet;
        packet.pid = SwapEndian<int>(pid);
        packet.address = SwapEndian<int>(address);
        packet.size = SwapEndian<int>(size);
        Socket::SendLargeData(s, &packet, sizeof(DebugReadWriteProcessMemoryPacket));
        Socket::SendLargeData(s, data, size);
        return closesocket(s), true;
    }
}