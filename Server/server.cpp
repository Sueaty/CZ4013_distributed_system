#include "stdafx.h"
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 9876
#define PACKET_SIZE 1024


int main(void){
	std::cout << "Server is now ON" << std::endl;

	WSADATA wsaData;
	SOCKET   socketS;
	SOCKADDR_IN  serverInfo;
	SOCKADDR_IN  clientInfo;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
		std::cout << "Failed to Initialize WinSock" << std::endl;
		std::cout << "Terminating program" << std::endl;
		WSACleanup();
		exit(0);
	}
	std::cout << "Successfully Initialized WinSock" << std::endl;

	// Successful in initializing WinSock
	socketS = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketS == INVALID_SOCKET) {
		std::cout << "Failed to create a socket" << std::endl;
		std::cout << "Terminating program" << std::endl;
		closesocket(socketS);
		WSACleanup();
		exit(0);
	}
	std::cout << "Successfully created a server socket" << std::endl;

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(PORT);
	serverInfo.sin_addr.s_addr = htonl(INADDR_ANY);

	int bindSocket = bind(socketS, (SOCKADDR*)&serverInfo, sizeof(serverInfo));

	if (bindSocket == SOCKET_ERROR) {
		std::cout << "Failed to bind address information to socket" << std::endl;
		std::cout << "Terminating program" << std::endl;
		closesocket(socketS);
		WSACleanup();
		exit(0);
	}
	std::cout << "Successfully Binded" << std::endl;

	int clientInfoSize = sizeof(clientInfo);
	char Buffer[PACKET_SIZE] = {};

	int recvSize = recvfrom(socketS, Buffer, PACKET_SIZE, 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
	if (recvSize == -1) {
		std::cout << "Failed to receive message" << std::endl;
		std::cout << "Terminating program" << std::endl;
		closesocket(socketS);
		WSACleanup();
		exit(0);
	}
	std::cout << "Packet received" << std::endl;
	//std::cout << "SENDER : " << inet_ntoa(clientInfo.sin_addr) << std::endl;

	std::cout << "DATA : " << Buffer << std::endl;

	char cMsg[] = "THIS IS SERVER SENDING MESSAGE";
	int sendSize = sendto(socketS, cMsg, strlen(cMsg), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
	if (sendSize != recvSize) {
		std::cout << "Failed to send message correctly" << std::endl;
		std::cout << "Terminating program" << std::endl;
		closesocket(socketS);
		WSACleanup();
		exit(0);
	}
	std::cout << "Successfully sent message" << std::endl;

	closesocket(socketS);
	WSACleanup();
}