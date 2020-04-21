
#include "stdafx.h"
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 9876
#define PACKET_SIZE 1024
#define SERVER_IP "192.168.0.166"

int main(void) {
	std::cout << "Client is now ON" << std::endl;

	WSADATA wsaData;
	SOCKET socketC;
	SOCKADDR_IN serverInfo;
	SOCKADDR_IN clientInfo;


	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
		std::cout << "Failed to Initialize WinSock" << std::endl;
		std::cout << "Terminating program" << std::endl;
		WSACleanup();
		exit(0);
	}
	std::cout << "Successfully Initialized WinSock" << std::endl;

	socketC = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketC == INVALID_SOCKET) {
		std::cout << "Failed to create a socket" << std::endl;
		std::cout << "Terminating program" << std::endl;
		closesocket(socketC);
		WSACleanup();
		exit(0);
	}
	std::cout << "Successfully created a server socket" << std::endl;

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(PORT);
	serverInfo.sin_addr.s_addr = inet_addr(SERVER_IP);

	// packet send
	char cMsg[] = "THIS IS CLIENT SENDING MESSAGE";
	int sendSize = sendto(socketC, cMsg, strlen(cMsg), 0, (struct sockaddr*) & serverInfo, sizeof(serverInfo));
	if (sendSize != strlen(cMsg)) {
		std::cout << "Failed to send packet" << std::endl;
		std::cout << "Terminating program" << std::endl;
		closesocket(socketC);
		WSACleanup();
		exit(0);
	}
	std::cout << "Succefully sent the packet" << std::endl;

	int serverInfoSize = sizeof(serverInfo);
	char Buffer[PACKET_SIZE] = {};
	int recvSize = recvfrom(socketC, Buffer, PACKET_SIZE, 0, (struct sockaddr*)&serverInfo, &serverInfoSize);
	if (recvSize == -1) {
		std::cout << "Failed to receive message" << std::endl;
		std::cout << "Terminating program" << std::endl;
		closesocket(socketC);
		WSACleanup();
		exit(0);
	}
	std::cout << "Packet received" << std::endl;
	//std::cout << "SENDER : " << inet_ntoa(serverInfo.sin_addr) << std::endl;
	std::cout << "DATA : " << Buffer << std::endl;

	closesocket(socketC);
	WSACleanup();

	return 0;
}