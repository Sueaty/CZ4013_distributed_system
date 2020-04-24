#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <string>
#include <WinSock2.h> //header file for sockets

#pragma comment(lib, "ws2_32") // LINK header file

#define PORT 9876
#define PACKET_SIZE 1024

/* ===== global variables ===== */
WSADATA wsaData;
SOCKET socketS; 
SOCKADDR_IN serverInfo, clientInfo;
int clientInfoSize = sizeof(clientInfo);

/* ===== helper functions ===== */
void openSocket(void);
void recvErrorDetect(int);
int sendMessage(char*);

/* ===== user request handling functions ===== */
std::string readFileAction(char*, int, int);
void readWriteFileAction(std::string, int, std::string);
void monitor(int);
int whitespaceCount(std::string);
void appendFileAction(std::string, std::string);


int main(void) {
	std::cout << "server: server on" << std::endl;
	openSocket();

	int option = 0;
	while (true) {
		int recvSize = recvfrom(socketS, (char*)&option, sizeof(int), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
		recvErrorDetect(recvSize);

		switch (option) {

		case 1: {
			char opt1[] = "server: option ONE selected";
			std::cout << opt1 << std::endl;
			sendMessage(opt1);

			typedef struct readFile {
				char filePath[200];
				int startByte;
				int numByteToRead;
			}readFile;

			readFile rf;
			recvSize = recvfrom(socketS, (char*)&rf, sizeof(rf), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			recvErrorDetect(recvSize);

			std::cout << rf.filePath << std::endl;
			std::cout << rf.startByte << std::endl;
			std::cout << rf.numByteToRead << std::endl;

			char recved[] = "server : Read file";
			sendMessage(recved);

			// Do the action
			std::string content;
			FILE* inFile;
			errno_t err = fopen_s(&inFile, rf.filePath, "r");
			if (err != 0) {
				std::cout << "server : cannot open file" << std::endl;
				exit(1);
			}
			int c = 0;
			for (int i = 0; i < rf.startByte; i++) //skip until startByte
				c = fgetc(inFile);
			for (int i = 0; i < rf.numByteToRead; i++) {
				c = fgetc(inFile);
				content += (char)c;
			}
			std::cout << "server : " << content << std::endl;

			const char* buff = content.c_str();
			int sendSize = sendto(socketS, buff, strlen(buff), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
			fclose(inFile);
			break;
		}

		case 2: {
			char opt2[] = "server: option TWO selected";
			std::cout << opt2 << std::endl;
			sendMessage(opt2);

			typedef struct readWriteFile {
				char filePath[200];
				int startByte;
				char content[PACKET_SIZE];
			}readWriteFile;

			readWriteFile rwf;
			recvSize = recvfrom(socketS, (char*)&rwf, sizeof(rwf), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			recvErrorDetect(recvSize);

			std::cout << rwf.filePath << std::endl;
			std::cout << rwf.startByte << std::endl;
			std::cout << rwf.content << std::endl;

			char recved[] = "server : Read, Write file";
			sendMessage(recved);

			// Do the action
			FILE* inFile;
			errno_t err = fopen_s(&inFile, rwf.filePath, "r+");
			if (err != 0) {
				std::cout << "server : cannot open file" << std::endl;
				exit(1);
			}

			fseek(inFile, 0L, SEEK_END);
			int fileSize = ftell(inFile);
			char* buff = new char[fileSize + 1];
			memset(buff, 0, fileSize + 1);
			rewind(inFile);
			fread(buff, fileSize, 1, inFile);
			std::cout << buff << std::endl;

			int futureFileSize = fileSize + strlen(rwf.content) + 1;
			char* concatbuff = new char[futureFileSize];
			std::cout << "PASS1" << std::endl;
			memset(concatbuff, 0, futureFileSize);
			std::cout << "PASS2" << std::endl;

			std::cout << buff << std::endl;
			if (rwf.startByte == 0) {
				strcpy(concatbuff, rwf.content);
				strcat(concatbuff, buff);
				std::cout << concatbuff << std::endl;
			}
			else {
				std::cout << "PASS3" << std::endl;
				std::string copyFile(buff); // all contents are here
				std::string s1 = copyFile.substr(0, rwf.startByte);
				std::string s2 = copyFile.substr(rwf.startByte);
				std::cout << "PASS4" << std::endl;

				char* cs1 = new char[s1.length() + 1];
				std::cout << "PASS5" << std::endl;
				strcpy(cs1, s1.c_str());
				std::cout << "PASS6" << std::endl;
				std::cout << cs1 << std::endl;
				char* cs2 = new char[s2.length() + 1];
				strcpy(cs2, s2.c_str());
				std::cout << cs2 << std::endl;

				strcpy(concatbuff, cs1);
				strcat(concatbuff, rwf.content);
				strcat(concatbuff, cs2);
				std::cout << concatbuff << std::endl;
			}
			fclose(inFile);

			FILE* reFile;
			err = fopen_s(&reFile, rwf.filePath, "w");
			if (err != 0) {
				std::cout << "server : cannot open file" << std::endl;
				exit(1);
			}
			fwrite(concatbuff, strlen(concatbuff), 1, reFile);
			int sendSize = sendto(socketS, concatbuff, strlen(concatbuff), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
			fclose(reFile);
			break;
		}
		case 3: {
			char opt3[] = "server: option THREE selected";
			std::cout << opt3 << std::endl;
			sendMessage(opt3);
			break;
		}
		case 4: {
			char opt4[] = "server: option FOUR selected";
			std::cout << opt4 << std::endl;
			sendMessage(opt4);

			typedef struct fileInfo {
				char filePath[200];
				char requesterName[50];
			} fileInfo;

			fileInfo fi;
			recvSize = recvfrom(socketS, (char*)&fi, sizeof(fi), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			recvErrorDetect(recvSize);
			
			std::cout << fi.filePath << std::endl;
			std::cout << fi.requesterName << std::endl;

			char recved[] = "server : Count Whitespace";
			sendMessage(recved);

			// Do the action
			FILE* inFile;
			errno_t err = fopen_s(&inFile, fi.filePath, "r");
			if (err != 0) {
				std::cout << "server : cannot open file" << std::endl;
				exit(1);
			}
			int c, wspaceCnt = 0;
			while ((c = fgetc(inFile)) != EOF) {
				if ((char)c == ' ' || (char)c == '\t' || (char)c == '\n')
					wspaceCnt++;
			}
			std::string strCnt = std::to_string(wspaceCnt);
			char* buff = new char[strCnt.length() + 1];
			strcpy(buff, strCnt.c_str());
			int sendSize = sendto(socketS, buff, strlen(buff), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
			fclose(inFile);
			break;
		}
		case 5: {
			char opt5[] = "Option 5. is selected";
			std::cout << opt5 << std::endl;
			sendMessage(opt5);

			typedef struct fileAppend {
				char filePath[200];
				char appendName[50];
			} fileAppend;

			fileAppend fa;

			recvSize = recvfrom(socketS, (char*)&fa, sizeof(fa), 0, (struct sockaddr*) & clientInfo, &clientInfoSize);
			if (recvSize == -1) {
				std::cout << "Failed to receive message" << std::endl;
				std::cout << "Terminating program" << std::endl;
				closesocket(socketS);
				WSACleanup();
				exit(0);
			}
			std::cout << "Packet received" << std::endl;
			std::cout << fa.filePath << std::endl;
			std::cout << fa.appendName << std::endl;

			char recved[] = "File append request received";
			sendMessage(recved);

			// Do the action
			appendFileAction(fa.filePath, fa.appendName);

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

void recvErrorDetect(int recvSize) {
	if (recvSize == -1) {
		std::cout << "server : failed to recieve packet" << std::endl;
		closesocket(socketS);
		WSACleanup();
		exit(0);
	}
	else {
		std::cout << "server : received packet" << std::endl;
	}
}

int sendMessage(char* cMsg) {
	int sendSize = sendto(socketS, cMsg, strlen(cMsg), 0, (struct sockaddr*) & clientInfo, clientInfoSize);
	return sendSize;
}



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
	fclose(inFile);
	return cnt;
}

void appendFileAction(std::string filePath, std::string name) {
	FILE* inFile;
	errno_t err = fopen_s(&inFile, filePath.c_str(), "a+");
	if (err != 0) {
		std::cout << "cannot open file" << std::endl;
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
