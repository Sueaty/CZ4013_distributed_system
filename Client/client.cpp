#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 9876
#define PACKET_SIZE 1024
#define SERVER_IP "192.168.0.166"

int showOptions(void);
void sendOption(int);
int recvString(void);
void openSocket(void);

WSADATA wsaData;
SOCKET socketC;
SOCKADDR_IN serverInfo;
SOCKADDR_IN clientInfo;
int serverInfoSize = sizeof(serverInfo);

int main(void) {
	openSocket();

	while (true) {
		int choice = showOptions(); // let the user choose action
		sendOption(choice); // send option information to server
		recvString(); // receive message from the server : Option n. selected
		if (choice == 6) {
			recvString();
			break;
		}

		switch (choice) {
		case 1: {
			typedef struct readFile {
				char filePath[200];
				int startByte;
				int numByteToRead;
			}readFile;
			readFile rf;

			std::cout << "Input File Path : ";
			std::cin.ignore();
			std::cin.getline(rf.filePath, 200);
			std::cout << "Input starting position(byte) : ";
			std::cin >> rf.startByte;
			std::cout << "Input number of bytes to read : ";
			std::cin >> rf.numByteToRead;


			// Marshaling structure
			// length of file path
			int pathLen = strlen(rf.filePath);
			std::string strPathLen = std::to_string(pathLen);
			char pathLenByte[] = "0000";
			int j = 3;
			for (int i = strPathLen.length() - 1; i >= 0; i--)
				pathLenByte[j--] = strPathLen[i];

			// startByte -> 1234
			std::string strStartByte = std::to_string(rf.startByte);
			char startByteLen[] = "0000";
			j = 3;
			for (int i = strStartByte.length() - 1; i >= 0; i--)
				startByteLen[j--] = strStartByte[i];
			// numByteToRead -> 1234
			std::string strReadByte = std::to_string(rf.numByteToRead);
			char toReadLen[] = "0000";
			j = 3;
			for (int i = strReadByte.length() - 1; i >= 0; i--)
				toReadLen[j--] = strReadByte[i];

			// concat three information
			int bufferSize = sizeof(pathLenByte) * 3 + pathLen;
			char* buffer = new char[bufferSize];
			memset(buffer, 0, bufferSize);
			strcpy(buffer, pathLenByte);
			strcat(buffer, rf.filePath);
			strcat(buffer, startByteLen);
			strcat(buffer, toReadLen);

			// send buffer size first
			// marshal bufferSize;
			std::string strBufferSize = std::to_string(bufferSize);
			char bufferSizeLen[] = "0000";
			j = 3;
			for (int i = strBufferSize.length() - 1; i >= 0; i--)
				bufferSizeLen[j--] = strBufferSize[i];
			int sendSize = sendto(socketC, bufferSizeLen, 4, 0, (struct sockaddr*) & serverInfo, sizeof(serverInfo));
			
			//send structure
			sendSize = sendto(socketC, buffer, bufferSize, 0, (struct sockaddr*) & serverInfo, sizeof(serverInfo));
			if (sendSize != bufferSize) {
				std::cout << "client: Failed to send packet" << std::endl;
				std::cout << "client: Terminating program" << std::endl;
				closesocket(socketC);
				WSACleanup();
				exit(0);
			}
			std::cout << "client: Succefully sent the packet" << std::endl;
			recvString(); // "Read File information recieved"
			recvString(); // "Content"
			break;
		}
		case 2: {
			typedef struct readWriteFile {
				char filePath[200];
				int startByte;
				char content[PACKET_SIZE];
			}readWriteFile;
			readWriteFile rwf;
			std::cout << "Input File Path : ";
			std::cin.ignore();
			std::cin.getline(rwf.filePath, 200);
			std::cout << "Input starting position(byte) : ";
			std::cin >> rwf.startByte;
			/* Deal with error when statByte, numByteRead doesn't match fileSize*/
	//
	//
	//
			std::cin.ignore();
			std::cout << "Input content to write : ";
			std::cin.getline(rwf.content, PACKET_SIZE);

			int sendSize = sendto(socketC, (char*)&rwf, sizeof(rwf), 0, (struct sockaddr*) & serverInfo, sizeof(serverInfo));
			if (sendSize != sizeof(rwf)) {
				std::cout << "client: Failed to send packet" << std::endl;
				std::cout << "client: Terminating program" << std::endl;
				closesocket(socketC);
				WSACleanup();
				exit(0);
			}
			std::cout << "client: Succefully sent the packet" << std::endl;
			recvString(); // "Read  Write File information recieved"
			recvString(); // "Content"
			break;
		}
		case 3: {
			//
			break;
		}
		case 4: {
			// Idempotent operation : result doesn't change
			// Count whitespace : \n, \t, ' '

			typedef struct fileInfo {
				char filePath[200];
				char requesterName[50];
			} fileInfo;

			fileInfo fi;
			std::cout << "Input File Path : ";
			std::cin.ignore();
			std::cin.getline(fi.filePath, 200);
			std::cout << "Input Requester's Name : ";
			std::cin.getline(fi.requesterName, 50);

			int sendSize = sendto(socketC, (char*)&fi, sizeof(fi), 0, (struct sockaddr*) & serverInfo, sizeof(serverInfo));
			if (sendSize != sizeof(fi)) {
				std::cout << "client: Failed to send packet" << std::endl;
				std::cout << "client: Terminating program" << std::endl;
				closesocket(socketC);
				WSACleanup();
				exit(0);
			}
			std::cout << "Succefully sent the packet" << std::endl;
			recvString(); // "File info request received"
			// receive number of whitespaces
			char Buffer[PACKET_SIZE] = {};
			int recvSize = recvfrom(socketC, Buffer, PACKET_SIZE, 0, (struct sockaddr*) & serverInfo, &serverInfoSize);
			if (recvSize == -1) {
				std::cout << "client: Failed to receive message" << std::endl;
				std::cout << "client: Terminating program" << std::endl;
				closesocket(socketC);
				WSACleanup();
				exit(0);
			}
			std::cout << "Number of spaces : " << Buffer << std::endl;

			break;
		}
		case 5: {
			typedef struct fileAppend {
				char filePath[200];
				char appendName[50];
			} fileAppend;

			fileAppend fa;
			std::cout << "Input File Path : ";
			std::cin.ignore();
			std::cin.getline(fa.filePath, 200);
			std::cout << "Input Requester's Name : ";
			std::cin.getline(fa.appendName, 50);

			int sendSize = sendto(socketC, (char*)&fa, sizeof(fa), 0, (struct sockaddr*) & serverInfo, sizeof(serverInfo));
			if (sendSize != sizeof(fa)) {
				std::cout << "client: Failed to send packet" << std::endl;
				std::cout << "client: Terminating program" << std::endl;
				closesocket(socketC);
				WSACleanup();
				exit(0);
			}
			std::cout << "Succefully sent the packet" << std::endl;
			recvString(); // "File append request received"
			recvString(); // Content
			break;
		}
		default:
			continue;	
		}
	}
	closesocket(socketC);
	WSACleanup();
	
	return 0;
}

int showOptions(void) {
	int userInput;
	std::cout << "=========================" << std::endl;
	std::cout << "1. Read File Content" << std::endl;
	std::cout << "2. Insert File Content" << std::endl;
	std::cout << "3. Monitor File Updates" << std::endl;
	std::cout << "4. ide Content" << std::endl;
	std::cout << "5. Non-ide Content" << std::endl;
	std::cout << "6. Terminate Program" << std::endl;
	std::cout << "=========================" << std::endl;

	std::cout << "Enter Option : ";
	std::cin >> userInput;
	return userInput;
}

void sendOption(int choice) {
	// Marshal 'option' to byte array
	// ex. if option = 3) cur = 0003
	char cur[4] = {0, };
	std::string strNum = std::to_string(choice);
	for (int i = 0; i < 3; i++)
		cur[i] = '0';
	cur[3] = strNum[0];

	int sendSize = sendto(socketC, (char*)cur, sizeof(cur), 0, (struct sockaddr*) & serverInfo, sizeof(serverInfo));
}

void openSocket(void) {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
		std::cout << "client: Failed to Initialize WinSock" << std::endl;
		std::cout << "client: Terminating program" << std::endl;
		WSACleanup();
		exit(0);
	}
	std::cout << "client: Successfully Initialized WinSock" << std::endl;

	socketC = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketC == INVALID_SOCKET) {
		std::cout << "client: Failed to create a socket" << std::endl;
		std::cout << "client: Terminating program" << std::endl;
		closesocket(socketC);
		WSACleanup();
		exit(0);
	}
	std::cout << "client: Successfully created a server socket" << std::endl;

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(PORT);
	serverInfo.sin_addr.s_addr = inet_addr(SERVER_IP);
}

int recvString(void) {
	char Buffer[PACKET_SIZE] = {};
	int recvSize = recvfrom(socketC, Buffer, PACKET_SIZE, 0, (struct sockaddr*) & serverInfo, &serverInfoSize);

	// Unmarshal String
	// Buffer : [0000][MSG]
	int recMsgLen = 1000 * (Buffer[0] - '0') + 100 * (Buffer[1] - '0') + 10 * (Buffer[2] - '0') + Buffer[3] - '0';
	char *recMsg = Buffer + 4;

	if (recvSize == -1) {
		std::cout << "client: Failed to receive message" << std::endl;
		std::cout << "client: Terminating program" << std::endl;
		closesocket(socketC);
		WSACleanup();
		exit(0);
	}
	std::cout << "client: Packet received" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------\n"
			  << "[Message Length] " << recMsgLen << "\n"
			  << "[DATA] " << recMsg << "\n"
			  << "----------------------------\n" << std::endl;
	return recvSize;
}
