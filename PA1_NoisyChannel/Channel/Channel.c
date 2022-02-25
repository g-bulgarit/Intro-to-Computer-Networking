#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define LISTEN_PORT 6000
#define SEND_PORT 6001
#define DEBUG
#define MSG_SIZE 8000

int main(int argc, char* argv[]) {
	printf("-------[NOISY CHANNEL]------- \r\n\r\n");
	// Check for cmdline args
	char* flag = "";
	int noiseAmt = 0;
	int randomNoiseSeed = 0;

	if (argc <= 2) {
		printf("[INFO] A noise function flag was not provided.\r\n\tDefaulting to no noise in transmission!.\r\n\t");
		printf("To add random noise, use the -r <probability> <seed>, and for deterministic noise use -d <amount>.\r\n");
	}
	else {
		flag = argv[1];
		noiseAmt = atoi(argv[2]);
		if (argc == 4)
			randomNoiseSeed = atoi(argv[3]);
	}


	// Setup recv buffer
	char recvBuf[MSG_SIZE];

	// Set up connection details
	struct sockaddr_in sListen;
	sListen.sin_family = AF_INET;
	sListen.sin_port = htons(LISTEN_PORT);
	sListen.sin_addr.s_addr = inet_addr("127.0.0.1");

	struct sockaddr_in sSend;
	sSend.sin_family = AF_INET;
	sSend.sin_port = htons(SEND_PORT);
	sSend.sin_addr.s_addr = inet_addr("127.0.0.1");

	// initialize windows networking and check for errors.
	WSADATA wsaData;
	SOCKET listenSocket;
	SOCKET transmitSocket;

	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		printf("[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	// Create actual IPv4 TCP socket for listening
	if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("[ERR] Failed to create listening socket, error code %d\r\n", WSAGetLastError());
		exit(1);
	}



	// Bind listen socket
	int retcode = bind(listenSocket, (SOCKADDR*)&sListen, sizeof(sListen));
	int canListen = listen(listenSocket, 1);

	while (1) {

		// TODO: Test retcodes
		printf("[Start] Started listening to the recieve socket, waiting for sender\r\n");
	#ifdef DEBUG
		printf("[DEBUG] Noise flag %s, amt %d, seed %d\r\n", flag, noiseAmt, randomNoiseSeed);
	#endif


		int c = sizeof(struct sockaddr_in);
		SOCKET s = accept(listenSocket, (SOCKADDR*)&sListen, &c);
		int recv_bytes = recv(s, recvBuf, MSG_SIZE, 0);
		if (recv_bytes) {
			printf("\r\n>: Recieved %d bytes: %s\r\n", recv_bytes, recvBuf);
		}
		closesocket(s);

		// Add noise to the buffered data here


		// Create actual IPv4 TCP socket for sending
		if ((transmitSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			printf("[ERR] Failed to create transmitting socket, error code %d\r\n", WSAGetLastError());
			exit(1);
		}
		// Connect to server socket
		int listen_retcode = connect(transmitSocket, (SOCKADDR*)&sSend, sizeof(sSend));
		printf("[Success] Connected to reciever \r\n");
		int sent_bytes = send(transmitSocket, recvBuf, sizeof(recvBuf), 0);
		printf("[Success] Sent %d bytes\n", sent_bytes);
		closesocket(transmitSocket);

	}

	return 0;
}
