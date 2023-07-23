
#include <stdio.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        printf("Usage: <ip-address> <port>\n");
        return 0;
    }

    const char* serverIp   = argv[1];
    int         serverPort = atoi(argv[2]);

    // Initialize winsock
    WSADATA        wsData = { 0 };
    unsigned short wsVersion = MAKEWORD(2, 2);

    int wsOk = WSAStartup(wsVersion, &wsData);
    if (wsOk < 0)
    {
        printf("[Winsock]: Failed to startup : %p\n", wsOk);
        return wsOk;
    }

    // Create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        printf("[Socket]: Failed to create : %p\n", clientSocket);
        return clientSocket;
    }

    // Connect to the server 
    sockaddr_in serverAddress;
    ZeroMemory(&serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp, &serverAddress.sin_addr);

    int connectionStatus = connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connectionStatus < 0)
    {
        printf("[Client]: Error connecting to %s:%i : %p\n", serverIp, serverPort, connectionStatus);
        return connectionStatus;
    }

    printf("[Client]: Connected to %s:%i\n", serverIp, serverPort);

    while (true)
    {
        // Clear buffer and get console input
        char buffer[1024];
        ZeroMemory(buffer, 1024);

        printf("[Client]: Enter command to send : ");
        gets_s(buffer);

        // Handle 'exit' command
        if (strncmp(buffer, "exit", 4) == 0)
        {
            send(clientSocket, buffer, strlen(buffer), 0);
            break;
        }

        // Send command to the server
        send(clientSocket, buffer, strlen(buffer), 0);
        printf("[Client]: Waiting for server response...\n");

        // Clear buffer and receive server response
        ZeroMemory(buffer, 1024);
        recv(clientSocket, buffer, 1024, 0);

        printf("[Server]: '%s'\n", buffer);
    }

    // Close socket
    closesocket(clientSocket);

    // Terminate winsock
    WSACleanup();

    return 0;
}