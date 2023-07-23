
#include <sys/prx.h>
#include <sys/ppu_thread.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#pragma diag_suppress 77
SYS_MODULE_INFO(tcp-server-ps3, 0, 1, 1);
SYS_MODULE_START(module_start);
SYS_MODULE_STOP(module_stop);


// sysPrxForUser imports
extern "C"
{
    int   _sys_printf(const char* fmt, ...);
    int   _sys_strncmp(const char* str1, const char* str2, int n);
    int   _sys_strlen(const char* str);
    void* _sys_memset(void* ptr, int c, int size);
}

#define printf                _sys_printf
#define strncmp               _sys_strncmp
#define strlen                _sys_strlen
#define ZeroMemory(ptr, size) _sys_memset(ptr, 0, size)


int              serverPort     = 1738;
bool             serverRunning  = true;
sys_ppu_thread_t serverThreadId = SYS_PPU_THREAD_ID_INVALID;

void ServerListener(uint64_t)
{
    printf("[Server]: Thread started\n");

    // Create the server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        printf("[Socket]: Failed to create : %p\n", serverSocket);
        return sys_ppu_thread_exit(serverSocket);
    }

    // Setup and bind the server
    sockaddr_in serverAddress;
    ZeroMemory(&serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(serverPort);

    int bindStatus = bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (bindStatus < 0)
    {
        printf("[Server]: Failed to bind socket to server : %p\n", bindStatus);
        return sys_ppu_thread_exit(bindStatus);
    }

    // Handle client connection
    printf("[Server]: Waiting for a client to connect...\n");
    listen(serverSocket, 5);

    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket < 0)
    {
        printf("[Server]: Error accepting client request : %p\n", clientSocket);
        return sys_ppu_thread_exit(clientSocket);
    }

    printf("[Server]: Connected from %s\n", inet_ntoa(clientAddress.sin_addr));
    
    // Listen
    while (serverRunning)
    {
        // Receive client command
        char buffer[1024];
        ZeroMemory(buffer, 1024);

        recv(clientSocket, buffer, 1024, 0);

        // Handle 'exit' command
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("[Server]: Client has quit the session\n");
            break;
        }

        printf("[Client]: '%s'\n", buffer);
        send(clientSocket, buffer, strlen(buffer), 0);
    }

    // Close sockets
    socketclose(clientSocket);
    socketclose(serverSocket);

    printf("[Server]: Exiting thread...\n");

    sys_ppu_thread_exit(0);
}

extern "C"
{
    int module_start(size_t args, void *argv)
    {
        sys_ppu_thread_create(&serverThreadId, ServerListener, 0, 400, 8 * 1024, SYS_PPU_THREAD_CREATE_JOINABLE, "Server");
        return 0;
    }

    int module_stop(size_t args, void *argv)
    {
        serverRunning = false;

        uint64_t vptr;
        sys_ppu_thread_join(serverThreadId, &vptr);
        return 0;
    }
}