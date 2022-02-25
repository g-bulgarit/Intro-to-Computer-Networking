#include <stdio.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define DEFAULT_PORT 6000

int main() {
	printf("-------[SENDER]------- \r\n\r\n");
	// Set up connection details
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(DEFAULT_PORT);
	remote.sin_addr.s_addr = inet_addr("127.0.0.1");


	// initialize windows networking and check for errors.
	WSADATA wsaData;
	SOCKET txSocket;

	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		printf("[ERR] WSAStartup() failed.\n");
		exit(1);
	}

	// Create actual IPv4 TCP socket
	if ((txSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("[ERR] Failed to create socket, error code %d", WSAGetLastError());
		exit(1);
	}

	// Connect to server socket
	int listen_retcode = connect(txSocket, (SOCKADDR*)&remote, sizeof(remote));
	printf("[Start] Connected to the Noisy Channel\r\n");

	char msg[] = "Fuck Tel-Aviv University!";
	int sent_bytes = send(txSocket, msg, sizeof(msg), 0);
	printf("[Success] Sent %d bytes\n", sent_bytes);

	return 0;
}
