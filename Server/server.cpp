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
void readWriteFileAction(std::string, int startPos, std::string);
void monitor(int);
int whitespaceCount(std::string);


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
		case 2: {
			char opt2[] = "Option 2. is selected";
			std::cout << opt2 << std::endl;
			sendMessage(opt2);

			typedef struct readWriteFile {
				char filePath[200];
				int startByte;
				char content[PACKET_SIZE];
			}readWriteFile;
			readWriteFile rwf;
			recvSize = recvfrom(socketS, (char*)&rwf, sizeof(rwf), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			if (recvSize == -1) {
				std::cout << "Failed to receive message" << std::endl;
				std::cout << "Terminating program" << std::endl;
				closesocket(socketS);
				WSACleanup();
				exit(0);
			}
			std::cout << "Packet received" << std::endl;
			std::cout << rwf.filePath << std::endl;
			std::cout << rwf.startByte << std::endl;
			std::cout << rwf.content << std::endl;

			char recved[] = "Read Write File information received";
			sendMessage(recved);

			// Do the action
			readWriteFileAction(rwf.filePath, rwf.startByte, rwf.content);
			break;
		}
		case 3: {
			char opt3[] = "Option 3. is selected";
			std::cout << opt3 << std::endl;
			sendMessage(opt3);

			break;
		}
		case 4: {
			char opt4[] = "Option 4. is selected";
			std::cout << opt4 << std::endl;
			sendMessage(opt4);

			typedef struct fileInfo {
				char filePath[200];
				char requesterName[50];
			} fileInfo;

			fileInfo fi;

			recvSize = recvfrom(socketS, (char*)&fi, sizeof(fi), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			if (recvSize == -1) {
				std::cout << "Failed to receive message" << std::endl;
				std::cout << "Terminating program" << std::endl;
				closesocket(socketS);
				WSACleanup();
				exit(0);
			}
			std::cout << "Packet received" << std::endl;
			std::cout << fi.filePath << std::endl;
			std::cout << fi.requesterName << std::endl;

			char recved[] = "File Info request received";
			sendMessage(recved);

			// Do the action
			int whitespaceCnt = whitespaceCount(fi.filePath);
			
			std::string str = std::to_string(whitespaceCnt);
			char* cstr = new char[str.length() + 1];
			strcpy(cstr, str.c_str());
			sendMessage(cstr);

			break;
		}
		case 5: {
			//
			break;
		}
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
	FILE* inFile;
	errno_t err = fopen_s(&inFile, filepath, "r");
	/* if(err != 0) -> Deal with error when file cannot be opened*/
	if (err != 0) {
		std::cout << "Cannot open file" << std::endl;
		std::cout << "Ending program" << std::endl;
		exit(1);
	}

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

void readWriteFileAction(std::string path, int startPos, std::string content) {

	FILE* inFile;
	errno_t err = fopen_s(&inFile, path.c_str(), "r+");
	if (err != 0) {
		std::cout << "cannot open file" << std::endl;
		exit(1);
	}
	fseek(inFile, 0L, SEEK_END);
	int fileSize = ftell(inFile) + content.length();
	char* buffer = new char[fileSize];
	memset(buffer, 0, fileSize);

	fseek(inFile, startPos, SEEK_SET);
	fwrite(content.c_str(), strlen(content.c_str()), 1, inFile);

	rewind(inFile);
	fread(buffer, fileSize, 1, inFile);


	int sendSize = sendto(socketS, buffer, strlen(buffer), 0, (struct sockaddr*) & clientInfo, clientInfoSize);


	fclose(inFile);
}


/*OPTION 3


*/

int whitespaceCount(std::string filePath) {
	FILE* inFile;
	errno_t err = fopen_s(&inFile, filePath.c_str(), "r");
	if (err != 0) {
		std::cout << "cannot open file" << std::endl;
		exit(1);
	}

	int c, cnt = 0;
	while ((c = fgetc(inFile)) != EOF) {
		if ((char)c == ' ' || (char)c == '\t' || (char)c == '\n')
			cnt++;
	}
	std::cout << cnt << std::endl;
	return cnt;
}
