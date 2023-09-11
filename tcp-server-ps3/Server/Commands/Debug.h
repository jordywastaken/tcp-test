
namespace Debug
{
    void GetProcessList(int s)
    {
        DebugProcessListPacket processList = { 0 };
        DbgGetProcessList(processList.pid);

        Socket::SendLargeData(s, &processList, sizeof(DebugProcessListPacket));
    }

    void GetProcessName(int s)
    {
        int pid = 0;
        Socket::RecvInt(s, &pid);

        DebugProcessNamePacket processName = { 0 };
        DbgGetProcessName(pid, processName.name);

        Socket::SendLargeData(s, &processName, sizeof(DebugProcessNamePacket));
    }

    void ReadProcessMemory(int s)
    {
        auto* packet = Socket::ReceiveType<DebugReadWriteProcessMemoryPacket>(s);
        char* data = new char[packet->size];

        DbgReadProcessMemory(packet->pid, packet->address, data, packet->size);
        Socket::SendLargeData(s, data, packet->size);

        delete[] data;
        delete packet;
    }

    void WriteProcessMemory(int s)
    {
        auto* packet = Socket::ReceiveType<DebugReadWriteProcessMemoryPacket>(s);
        char* data = new char[packet->size];

        Socket::RecvLargeData(s, data, packet->size);
        DbgWriteProcessMemory(packet->pid, packet->address, data, packet->size);

        delete[] data;
        delete packet;
    }

    void AllocateProcessPage(int s)
    {

    }

    void FreeProcessPage(int s)
    {

    }
}