#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define LISTEN_PORT		6000
#define SEND_PORT		6001
#define MSG_SIZE		500000
#define BYTE_SIZE_IN_BITS 8
#define DEBUG


void flipBit(char* buffer, int index) {
	// Flip specific bit in buffer

	buffer[index / BYTE_SIZE_IN_BITS] ^= (1u << (7 - (index % BYTE_SIZE_IN_BITS)));

}

void addDeterministicNoise(int n, char* buffer, int bufSize, int* flippedBits) {
	for (int bitIdx = 0; bitIdx < bufSize * BYTE_SIZE_IN_BITS; bitIdx += n)
	{
		flipBit(buffer, bitIdx);
		*flippedBits += 1;
	}
}

void addRandomNoise(int prob, int seed, char* buffer, int bufferSize, int* flippedBits) {
	// For each bit in the message, flip it with probability 'prob'/2^16.
	for (int i = 0; i < bufferSize; i++){
		//rand();
		*flippedBits += 1;
	}

}

int main(int argc, char* argv[]) {
	// Check for cmdline args
	char* flag = "";
	int noiseAmt = 0;
	int randomNoiseSeed = 0;
	// User input
	char userInputBuffer[5];
	int amtBitsFlipped = 0;


	printf("-------[NOISY CHANNEL]------- \r\n\r\n");

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

	// Set up connection detail structs
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
		// TODO: Test retcodes here

	while (1) {

		printf("[Start] Started listening to the recieve socket, waiting for sender\r\n");
	#ifdef DEBUG
		printf("[DEBUG] Noise flag %s, amt %d, seed %d\r\n", flag, noiseAmt, randomNoiseSeed);
	#endif
		
		// Allocate (too much) memory for message
		char* recvBuf = (char*)malloc(sizeof(char) * MSG_SIZE);
		if (recvBuf == NULL) {
			printf("[ERR] Failed to allocate memory for recieving the message. Exiting");
			exit(1);
		}

		int c = sizeof(struct sockaddr_in);
		SOCKET s = accept(listenSocket, (SOCKADDR*)&sListen, &c);
		int recievedMessageSize = recv(s, recvBuf, MSG_SIZE, 0);

		recvBuf = (char*)realloc(recvBuf, recievedMessageSize + 1);
		recvBuf[recievedMessageSize + 1] = '\0';

		if (recievedMessageSize) {
			printf("\r\n>: Recieved %d bytes: \r\n%s\r\n", recievedMessageSize, recvBuf);
		}
		closesocket(s);

		// Add noise to the buffered data here
		if (!strcmp(flag, "-d")) {
			addDeterministicNoise(noiseAmt, recvBuf, recievedMessageSize, &amtBitsFlipped);
			printf("\r\nAdded deterministic noise!, flipped %d bits\r\n", amtBitsFlipped);
			printf("\r\n>After Noise: \r\n%s\r\n", recvBuf);
		}
		else if (!strcmp(flag, "-r")) {
			printf("\r\nAdded random noise!\r\n");
			// TODO add noise
		}


		// Create actual IPv4 TCP socket for sending
		if ((transmitSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			printf("[ERR] Failed to create transmitting socket, error code %d\r\n", WSAGetLastError());
			exit(1);
		}
		// Connect to server socket
		int listen_retcode = connect(transmitSocket, (SOCKADDR*)&sSend, sizeof(sSend));
		printf("[Success] Connected to reciever \r\n");
		int sentMessageSize = send(transmitSocket, recvBuf, recievedMessageSize, 0);
		printf("[Success] Sent %d bytes\n", sentMessageSize);
		closesocket(transmitSocket);

		printf(">: Continue? (yes/no)\r\n");
		amtBitsFlipped = 0;
		scanf("%s", &userInputBuffer);
		if (!strcmp(userInputBuffer, "no")) {
			exit(0);
		}

	}

	return 0;
}
