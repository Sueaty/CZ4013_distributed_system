#include "stdafx.h"
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <string>
#include <WinSock2.h> //header file for sockets

#pragma comment(lib, "ws2_32") // LINK header file

#define PORT 9876
#define PACKET_SIZE 1024

void openSocket(void);
int sendMessage(char*);

WSADATA wsaData; //structure that saves Windows's socket initialized info
SOCKET socketS; // SOCKET handle
// structure that contains elements that consist a socket
SOCKADDR_IN serverInfo, clientInfo;
int clientInfoSize = sizeof(clientInfo);
std::string readFileAction(char*, int, int);


int main(void) {
	std::cout << "Server is now ON" << std::endl;

	openSocket();
	

	int option, recvSize;
	while (true) {
		recvSize = recvfrom(socketS, (char*)&option, sizeof(int), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
		if (recvSize == -1) {
			std::cout << "Failed to recieve message" << std::endl;
			std::cout << "terminating program" << std::endl;
			closesocket(socketS);
			WSACleanup();
			exit(0);
		}
		std::cout << "Packet recieved" << std::endl;

		switch (option) {
		case 1: {
			char opt1[] = "Option 1. is selected";
			std::cout << opt1 << std::endl;
			sendMessage(opt1);

			typedef struct readFile {
				char filePath[200];
				int startByte;
				int numByteToRead;
			}readFile;
			readFile rf;
			recvSize = recvfrom(socketS, (char*)&rf, sizeof(rf), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			if (recvSize == -1) {
				std::cout << "Failed to receive message" << std::endl;
				std::cout << "Terminating program" << std::endl;
				closesocket(socketS);
				WSACleanup();
				exit(0);
			}
			std::cout << "Packet received" << std::endl;
			std::cout << rf.filePath << std::endl;
			std::cout << rf.startByte << std::endl;
			std::cout << rf.numByteToRead << std::endl;
			
			char recved[] = "Read File information received";
			sendMessage(recved);

			// Do the action
			std::string content = readFileAction(rf.filePath, rf.startByte, rf.numByteToRead);
			const char* buff = content.c_str();
			int sendSize = sendto(socketS, buff, strlen(content.c_str()), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
			break;
		}
		case 2:
			//
			break;
		case 3:
			//
			break;
		case 4:
			//
			break;
		case 5:
			//
			break;
		case 6: {
			std::cout << "End of Server-Client Program" << std::endl;
			char msg6[] = "Terminate Server-Client Program";
			sendMessage(msg6);
			closesocket(socketS);
			WSACleanup();
			break;
		}
		default: {
			std::cout << "Invalid option input" << std::endl;
			char msgD[] = "Invalid option number received";
			sendMessage(msgD);
		}
		}


		/*
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
		*/

		/*
		char cMsg[] = "Option Received";
		int sendSize = sendto(socketS, cMsg, strlen(cMsg), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
		
		if (sendSize != recvSize) {
			std::cout << "Failed to send message correctly" << std::endl;
			std::cout << "Terminating program" << std::endl;
			closesocket(socketS);
			WSACleanup();
			exit(0);
		}
		std::cout << "Successfully sent message" << std::endl;
		*/
	}
}

void openSocket(void) {
	// WSAStartup : tells whih socket in Windows I will use
	//				using socket 2.2 version
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
		// Error case : when it fails to start up
		std::cout << "Failed to Initialize WinSock" << std::endl;
		std::cout << "Terminating program" << std::endl;
		WSACleanup();
		exit(0);
	}
	std::cout << "Successfully Initialized WinSock" << std::endl;

	// Successful in initializing WinSock
	socketS = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketS == INVALID_SOCKET) {
		// Error case : when it fails to create socket
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

	// Bind : connecting address information with socket
	int bindSocket = bind(socketS, (SOCKADDR*)&serverInfo, sizeof(serverInfo));
	if (bindSocket == SOCKET_ERROR) {
		// Error case : when it fails to bind socket with address
		std::cout << "Failed to bind address information to socket" << std::endl;
		std::cout << "Terminating program" << std::endl;
		closesocket(socketS);
		WSACleanup();
		exit(0);
	}
	std::cout << "Successfully Binded" << std::endl;
}

int sendMessage(char* cMsg) {
	int sendSize = sendto(socketS, cMsg, strlen(cMsg), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
	return sendSize;
}

/* ========== ACTION ==========*/
std::string readFileAction(char* filepath, int startByte, int numByteRead) {
	std::cout << "Pos here" << std::endl;
	FILE* inFile;
	errno_t err = fopen_s(&inFile, filepath, "r");
	/* if(err != 0) -> Deal with error when file cannot be opened*/
	if (err != 0) {
		std::cout << "Cannot open file" << std::endl;
		std::cout << "Ending program" << std::endl;
		exit(1);
	}
	//fseek(inFile, 0L, SEEK_END);
	//int fileSize = ftell(inFile);
	std::cout << "opened file" << std::endl;
	/* Deal with error when statByte, numByteRead doesn't match fileSize*/
	int c = 0;
	std::string content;
	for (int i = 0; i < startByte; i++)
		c = fgetc(inFile);
	for (int i = 0; i < numByteRead; i++) {
		c = fgetc(inFile);
		content += (char)c;
	}
	std::cout << content << std::endl;
	return content;
}
