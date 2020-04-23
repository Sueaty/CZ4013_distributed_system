#include "stdafx.h"
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
void readWriteFileAction(std::string, int, std::string);
void monitor(int);
int whitespaceCount(std::string);
void appendFileAction(std::string, std::string);


int main(void) {
	std::cout << "Server is now ON" << std::endl;

	openSocket();
	
	int option = 0, recvSize;
	while (true) {
		//Unmarshal INT sent as user's choice
		char cur[4] = {};
		recvSize = recvfrom(socketS, (char*)cur, sizeof(cur), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
		option = cur[3] - '0'; // char -> int

		if (recvSize == -1) {
			std::cout << "server: Failed to recieve message" << std::endl;
			std::cout << "server: terminating program" << std::endl;
			closesocket(socketS);
			WSACleanup();
			exit(0);
		}
		std::cout << "server: Packet recieved" << std::endl;

		if (option == 6) {
			char opt6[] = "server : selected option is SIX";
			std::cout << opt6 << std::endl;
			sendMessage((char*)opt6);

			char msg6[] = "server: Terminate Server-Client Program";
			sendMessage(msg6);
			closesocket(socketS);
			WSACleanup();
			break;
		
		}

		switch (option) {
		case 1: {
			char opt1[] = "server : selected option is ONE";
			std::cout << opt1 << std::endl;
			sendMessage((char*)opt1);

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
			
			char recved[] = "server: Read File information received";
			sendMessage(recved);

			// Do the action
			std::string content = readFileAction(rf.filePath, rf.startByte, rf.numByteToRead);
			//const char* buff = content.c_str();
			//int sendSize = sendto(socketS, buff, strlen(content.c_str()), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
			char* buffer = new char[content.length() + 1];
			strcpy(buffer, content.c_str());
			sendMessage(buffer);
			delete[] buffer;
			break;
		}
		case 2: {
			char opt2[] = "server : selected option is TWO";
			std::cout << opt2 << std::endl;
			sendMessage((char*)opt2);

			typedef struct readWriteFile {
				char filePath[200];
				int startByte;
				char content[PACKET_SIZE];
			}readWriteFile;
			readWriteFile rwf;
			recvSize = recvfrom(socketS, (char*)&rwf, sizeof(rwf), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			if (recvSize == -1) {
				std::cout << "server: Failed to receive message" << std::endl;
				std::cout << "server: Terminating program" << std::endl;
				closesocket(socketS);
				WSACleanup();
				exit(0);
			}
			std::cout << "Packet received" << std::endl;
			std::cout << rwf.filePath << std::endl;
			std::cout << rwf.startByte << std::endl;
			std::cout << rwf.content << std::endl;

			char recved[] = "server: Read Write File information received";
			sendMessage(recved);

			// Do the action
			readWriteFileAction(rwf.filePath, rwf.startByte, rwf.content);
			break;
		}
		case 3: {
			char opt3[] = "server : selected option is THREE";
			std::cout << opt3 << std::endl;
			sendMessage((char*)opt3);

			break;
		}
		case 4: {
			char opt4[] = "server : selected option is FOUR";
			std::cout << opt4 << std::endl;
			sendMessage((char*)opt4);

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
			char opt5[] = "server : selected option is FIVE";
			std::cout << opt5 << std::endl;
			sendMessage((char*)opt5);

			typedef struct fileAppend {
				char filePath[200];
				char appendName[50];
			} fileAppend;

			fileAppend fa;

			recvSize = recvfrom(socketS, (char*)&fa, sizeof(fa), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			if (recvSize == -1) {
				std::cout << "server: Failed to receive message" << std::endl;
				std::cout << "server: Terminating program" << std::endl;
				closesocket(socketS);
				WSACleanup();
				exit(0);
			}
			std::cout << "server: Packet received" << std::endl;
			std::cout << fa.filePath << std::endl;
			std::cout << fa.appendName << std::endl;

			char recved[] = "server: File append request received";
			sendMessage(recved);

			// Do the action
			appendFileAction(fa.filePath, fa.appendName);

			break;
		}
		default: {
			char msgD[] = "server : Invalid option input";
			std::cout << msgD << std::endl;
			sendMessage((char*)msgD);
		}
		}
	}
}

void openSocket(void) {
	// WSAStartup : tells whih socket in Windows I will use
	//				using socket 2.2 version
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
		// Error case : when it fails to start up
		std::cout << "server: Failed to Initialize WinSock" << std::endl;
		std::cout << "server: Terminating program" << std::endl;
		WSACleanup();
		exit(0);
	}
	std::cout << "server: Successfully Initialized WinSock" << std::endl;

	// Successful in initializing WinSock
	socketS = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketS == INVALID_SOCKET) {
		// Error case : when it fails to create socket
		std::cout << "server: Failed to create a socket" << std::endl;
		std::cout << "server: Terminating program" << std::endl;
		closesocket(socketS);
		WSACleanup();
		exit(0);
	}
	std::cout << "server: Successfully created a server socket" << std::endl;

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(PORT);
	serverInfo.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind : connecting address information with socket
	int bindSocket = bind(socketS, (SOCKADDR*)&serverInfo, sizeof(serverInfo));
	if (bindSocket == SOCKET_ERROR) {
		// Error case : when it fails to bind socket with address
		std::cout << "server: Failed to bind address information to socket" << std::endl;
		std::cout << "server: Terminating program" << std::endl;
		closesocket(socketS);
		WSACleanup();
		exit(0);
	}
	std::cout << "server: Successfully Binded" << std::endl;
}

int sendMessage(char* cMsg) {
	// char*buffer's format : [4byte : length info][cMsg];
	int msgLen = strlen(cMsg);
	std::string strMsgLen = std::to_string(msgLen);

	char msgLenByte[] = "0000";

	int j = 3;
	for (int i = strMsgLen.length() - 1; i >= 0; i--) {
		msgLenByte[j--] = strMsgLen[i];
	}

	int bufferSize = sizeof(msgLenByte) + msgLen;
	char* buffer = new char[bufferSize];
	memset(buffer, 0, sizeof(msgLenByte) + msgLen);
	strcpy(buffer, msgLenByte);
	strcat(buffer, cMsg);

	int sendSize = sendto(socketS, buffer, bufferSize, 0, (struct sockaddr*) & clientInfo, clientInfoSize);

	return sendSize;
}

/* ========== ACTION ==========*/
std::string readFileAction(char* filepath, int startByte, int numByteRead) {
	FILE* inFile;
	errno_t err = fopen_s(&inFile, filepath, "r");
	/* if(err != 0) -> Deal with error when file cannot be opened*/
	if (err != 0) {
		std::cout << "server: Cannot open file" << std::endl;
		std::cout << "server: Ending program" << std::endl;
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
		std::cout << "server: cannot open file" << std::endl;
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

	sendMessage(buffer);
	fclose(inFile);
}


/*OPTION 3


*/

int whitespaceCount(std::string filePath) {
	FILE* inFile;
	errno_t err = fopen_s(&inFile, filePath.c_str(), "r");
	if (err != 0) {
		std::cout << "server: cannot open file" << std::endl;
		exit(1);
	}

	int c, cnt = 0;
	while ((c = fgetc(inFile)) != EOF) {
		if ((char)c == ' ' || (char)c == '\t' || (char)c == '\n')
			cnt++;
	}
	std::cout << cnt << std::endl;
	fclose(inFile);
	return cnt;
}

void appendFileAction(std::string filePath, std::string name) {
	FILE* inFile;
	errno_t err = fopen_s(&inFile, filePath.c_str(), "a+");
	if (err != 0) {
		std::cout << "server: cannot open file" << std::endl;
		exit(1);
	}
	fputs(name.c_str(), inFile);

	fseek(inFile, 0L, SEEK_END);
	int fileSize = ftell(inFile) + name.length();
	char* buffer = new char[fileSize];
	memset(buffer, 0, fileSize);
	rewind(inFile);
	fread(buffer, fileSize, 1, inFile);
	std::cout << buffer << std::endl;
	int sendSize = sendto(socketS, buffer, strlen(buffer), 0, (struct sockaddr*) & clientInfo, clientInfoSize);

	fclose(inFile);
}
