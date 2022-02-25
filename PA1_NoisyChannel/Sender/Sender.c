#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library

int main(int argc, char* argv[]) {
	int stopUserInput = 0;
	char fileNameBuffer[3000];

	printf("-------[SENDER]------- \r\n\r\n");


	// Check for cmdline args
	if (argc != 3) {
		printf("[ERR] Please provide all required command-line arguments.\r\n. Sender.exe <ip_address> <port>\r\n");
		exit(1);
	}

	// Set up connection details
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(atoi(argv[2]));
	remote.sin_addr.s_addr = inet_addr(argv[1]);

	#ifdef DEBUG
	printf("[DEBUG] Starting sender with IP %s and port %d\r\n", argv[1], remote.sin_port);
	#endif

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

#ifdef DEBUG
	printf("[Start] Connected to the Noisy Channel\r\n");
#endif
	while (!stopUserInput) {
		printf(">: Enter file name: ");
		scanf("%s", &fileNameBuffer);

		if (!strcmp(fileNameBuffer, "quit"))
			exit(0);

		// Check if the file exists and open it.

;		int sent_bytes = send(txSocket, fileNameBuffer, sizeof(fileNameBuffer), 0);
		printf(">: Sent: %d bytes\n", sent_bytes);

	}

	return 0;
}
