#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

void main(int argc, char* argv[]) //we can pass in data using command line
{
	// Start Winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsStarted = WSAStartup(version, &data);
	if (wsStarted != 0) {
		std::cout << "Can't start windsock" << wsStarted;
		return;
	}

	// Create a hint structure for the server
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(54000);
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);


	// Socket creation
	SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

	// WRite out to that socket
	std::string s(argv[1]);
	int sendOk = sendto(out, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, sizeof(server));

	if (sendOk == SOCKET_ERROR) {
		std::cout << "That didn't work! " << WSAGetLastError() << std::endl;
	}
	// Close the socket
	closesocket(out);
	// Close down winsock
	WSACleanup();
}
