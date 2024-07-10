#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

void main()
{
	// Start Winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsStarted = WSAStartup(version, &data);
	if (wsStarted != 0) {
		std::cout << "Can't start windsock" << wsStarted; 
		return;
	}

	// Bind socket to IP address and port
	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Just get any address from network card
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(54000); // Convert little to big endian

	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR) {
		std::cout << "Can't bind to socket! " << WSAGetLastError() << std::endl;
	}

	// Client data??
	sockaddr_in client;
	int clientLength = sizeof(client);
	ZeroMemory(&client, clientLength);

	// Message buffer??
	char buf[1024];
	

	// Enter loop
	while (true) {
		ZeroMemory(buf, 1024);
		
		// Wait for message
		int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR) {
			std::cout << "Error receiving from client " << WSAGetLastError() << std::endl;
			continue;
		}
		// Display message and client client info
		char clientIp[256];
		ZeroMemory(clientIp, 256);

		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		std::cout << "message recv from " << clientIp << " : " << buf << std::endl;
	}
		
		

	// Close socket
	closesocket(in);
	// shutdown winsock
	WSACleanup();
}
