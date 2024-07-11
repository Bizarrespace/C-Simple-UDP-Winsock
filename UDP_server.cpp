#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <windows.h>
#include <chrono>
#include <thread>
#pragma comment (lib, "ws2_32.lib")

// Color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

void printColored(const char* color, const std::string& message) {
    std::cout << color << message << RESET << std::endl;
}

int main()
{
    // Enable Windows console virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // Start Winsock
    WSADATA data;
    WORD version = MAKEWORD(2, 2);
    int wsStartup = WSAStartup(version, &data);
    if (wsStartup != 0) {
        printColored(RED, "Can't start Winsock");
        return 1;
    }

    // Create socket
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serverHint;
    serverHint.sin_addr.S_un.S_addr = ADDR_ANY;

    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(54000);

    // Bind socket
    if (bind(serverSocket, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR) {

        printColored(RED, "Can't bind to socket!");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printColored(GREEN, "Server is listening on port 54000...");

    // Client meta data
    sockaddr_in client;
    int clientLength = sizeof(client);
    char buf[1024];
    std::string lastClientId;
    auto lastMessageTime = std::chrono::steady_clock::now();
    const int TIMEOUT_SECONDS = 10;

    // Set socket to non-blocking mode
    u_long mode = 1;
    ioctlsocket(serverSocket, FIONBIO, &mode);


    while (true) {
        ZeroMemory(buf, 1024);
        ZeroMemory(&client, clientLength);

        // Try to receive a message
        int bytesIn = recvfrom(serverSocket, buf, 1024, 0, (sockaddr*)&client, &clientLength);
        // Receive information using the server socket, in to the buffer, and then we have the client structure, and its length

        if (bytesIn > 0) {
            char clientIp[256];
            ZeroMemory(clientIp, 256);
            inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);
            unsigned short clientPort = ntohs(client.sin_port);
            std::string clientId = std::string(clientIp) + ":" + std::to_string(clientPort);

            if (clientId != lastClientId) {
                printColored(BLUE, "Client connected: " + clientId);
                lastClientId = clientId;
            }

            std::cout << YELLOW << "Message from " << clientId << ": " << RESET << buf << std::endl;
            lastMessageTime = std::chrono::steady_clock::now();
        }
        else if (WSAGetLastError() != WSAEWOULDBLOCK) {
            printColored(RED, "Error receiving from client");
        }

        // Check for timeout
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastMessageTime);

        if (!lastClientId.empty() && elapsed.count() > TIMEOUT_SECONDS) {
            printColored(BLUE, "Client disconnected (timeout): " + lastClientId);
            lastClientId.clear();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
