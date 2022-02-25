#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define MSG_SIZE 500000

int main(int argc, char* argv[]) {
	FILE* wfp;
	char fileNameBuffer[3000];

	printf("-------[RECIEVER]------- \r\n\r\n");
	// Check for cmdline args
	if (argc != 3) {
		printf("[ERR] Please provide all required command-line arguments.\r\n Reciever.exe <ip_address> <port>\r\n");
		exit(1);
	}

	// Set up connection details
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));
	server.sin_addr.s_addr = inet_addr(argv[1]);

	struct sockaddr_in client;

	// initialize windows networking and check for errors.
	WSADATA wsaData;
	SOCKET rxSocket;

	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		printf("[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	// Create actual IPv4 TCP socket
	if ((rxSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("[ERR] Failed to create socket, error code %d\r\n", WSAGetLastError());
		exit(1);
	}

	// Bind socket
	int retcode = bind(rxSocket, (SOCKADDR*)&server, sizeof(server));
	int listen_retcode = listen(rxSocket, 1);
	// TODO: Test retcodes

	// Setup recv buffer
	char recvBuf[MSG_SIZE];
	printf("[Start] Started socket, listening to connections from the noisy channel\r\n");

	while (1) {
		int c = sizeof(struct sockaddr_in);
		SOCKET s = accept(rxSocket, (SOCKADDR*)&client, &c);
		int recv_bytes = recv(s, recvBuf, MSG_SIZE, 0);
		if (recv_bytes) {
			printf("[Success] Recieved %d bytes\r\n", recv_bytes);
			printf("[Success] Recieved:\r\n%s\r\n", recvBuf);
		}

		// Decode the file here

		// Save the file with the help of some user input
		printf(">: Enter filename: ");
		scanf("%s", &fileNameBuffer);
		wfp = fopen(fileNameBuffer, "wb+");
		fwrite(recvBuf, 1, MSG_SIZE, wfp);
		fclose(wfp);
		closesocket(s);
	}

	return 0;
}
