#include <iostream>
#include <WS2tcpip.h>
#include <string>
#pragma comment (lib, "ws2_32.lib")
int main()
{
	// Start Winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsStarted = WSAStartup(version, &data);
	if (wsStarted != 0) {
		std::cout << "Can't start windsock" << wsStarted;
		return 1;
	}

	// Create a hint structure for the server
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(54000);
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);


	// Socket creation
	SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

	// WRite out to that socket
	std::string userInput;
	do {
		std::cout << "Enter a message (or 'quit' to exit): ";
		std::getline(std::cin, userInput);

		if (userInput != "quit") {
			// Send the message that the user typed
			int sendOk = sendto(out, userInput.c_str(), userInput.size() + 1, 0, (sockaddr*)&server, sizeof(server));
			if (sendOk == SOCKET_ERROR) {
				std::cout << "Error sending message" << WSAGetLastError() << std::endl;
			}
		}
	} while (userInput != "quit");

	// Close the socket
	closesocket(out);
	// Close down winsock
	WSACleanup();

	return 0;
}