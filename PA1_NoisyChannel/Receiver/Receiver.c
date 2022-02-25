#include <stdio.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define DEFAULT_PORT 6003
#define MSG_SIZE 300

int main() {
	// Setup recv buffer
	char recvBuf[2500];

	// Set up connection details
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(DEFAULT_PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

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

	printf("[Start] Started socket for Reciever\r\n");

	while (1) {
		int c = sizeof(struct sockaddr_in);
		SOCKET s = accept(rxSocket, (SOCKADDR*)&client, &c);
		int recv_bytes = recv(s, recvBuf, MSG_SIZE, 0);
		if (recv_bytes) {
			printf("[Success] Recieved %d bytes\r\n", recv_bytes);
			printf("[Success] Recieved: %s\r\n", recvBuf);
		}
	}

	return 0;
}
