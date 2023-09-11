
#include "API.h"
#include "Common/Imports.h"
#include "Common/Socket.h"
#include "Common/ThreadPool.h"
#include "Common/Syscalls.h"
#include "Commands/General.h"
#include "Commands/Debug.h"

struct ClientData
{
    sockaddr_in address;
    int socket;
};

namespace API
{
    bool Running;

    CommandHandler CommandTable[API_CMD_COUNT]
    {
        General::TestConnectivity,
        General::Notify,

        Debug::GetProcessList,
        Debug::GetProcessName,
        Debug::ReadProcessMemory,
        Debug::WriteProcessMemory,
    };

    void ClientCallback(void* userdata)
    {
        ClientData* client = reinterpret_cast<ClientData*>(userdata);
        auto* packet = Socket::ReceiveType<InitialPacket>(client->socket);

        if (packet)
        {
            // Verify packet header
            if (strncmp(packet->magic, PacketMagic, 8) || packet->version != API_VERSION)
            {
                DebugPrint("API", "Invalid packet received with magic '%s' and version %i. Expected: magic '%s' and version %i\n", packet->magic, packet->version, PacketMagic, API_VERSION);
                Socket::SendInt(client->socket, API_ERROR_INVALID_PACKET);
                __builtin_trap();
            }

            DebugPrint("API", "Received command %i from %s\n", packet->cmd, inet_ntoa(client->address.sin_addr));
            Socket::SendInt(client->socket, API_OK);

            CommandTable[packet->cmd](client->socket);
            delete packet;
        }

        socketclose(client->socket);
        delete client;
    }

    void ListenerCallback(void*)
    {
        int serverSocket = Socket::SetupServer(API_PORT, 1024);

        if (serverSocket < 0)
        {
            DebugPrint("API", "Failed to startup the server...\n");
            return;
        }

        DebugPrint("API", "Server started, listening on port %i\n", API_PORT);

        while (Running)
        {
            int stat = Socket::Stat(serverSocket);

            if (stat == -1)
            {
                DebugPrint("API", "Stat error...\n");
                Socket::Close(serverSocket);
                return;
            }

            if (!Running)
                goto Cleanup;

            if (stat == 0)
                continue;

            ClientData client;
            client.socket = Socket::AcceptNewConnection(serverSocket, &client.address);

            if (client.socket < 0)
                continue;

            ThreadPool::QueueJob(ClientCallback, new ClientData(client));
        }

    Cleanup:
        DebugPrint("API", "Server stopped\n");
        Socket::Close(serverSocket);
    }

    void Initialize()
    {
        Running = true;
        ThreadPool::QueueJob(ListenerCallback, 0);
    }

    void Terminate()
    {
        Running = false;
    }
}