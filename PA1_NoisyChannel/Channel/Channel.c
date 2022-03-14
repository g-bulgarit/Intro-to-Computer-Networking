// General Imports
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Winsock Imports
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library

// Constants and other stuff
#define LISTEN_PORT		6000
#define SEND_PORT		6001
#define MSG_SIZE		500000
#define BYTE_SIZE_IN_BITS 8
#define DEBUG

#define LISTEN			1
#define SEND			2


void flipBit(char* buffer, int index) {
	// Flip specific bit in buffer
	buffer[index / BYTE_SIZE_IN_BITS] ^= (1u << (7 - (index % BYTE_SIZE_IN_BITS)));
}

void addDeterministicNoise(int n, char* buffer, int bufSize, int* flippedBits) {
	// Flip every n-th bit in the message.
	for (int bitIdx = 0; bitIdx < bufSize * BYTE_SIZE_IN_BITS; bitIdx += n)
	{
		flipBit(buffer, bitIdx);
		*flippedBits += 1;
	}
}

void addRandomNoise(int prob, int seed, char* buffer, int bufferSize, int* flippedBits) {
	// For each bit in the message, flip it with probability 'prob'/2^16.
	int cap = pow(2,16);
	srand(seed);
	for (int i = 0; i < bufferSize * BYTE_SIZE_IN_BITS; i++){
		int doFlip = (rand() % cap) < prob;
		if (doFlip) {
			flipBit(buffer, i);
			*flippedBits += 1;
		}
	}
}

SOCKET createSocket(char* ipAddress, int port, int mode, struct sockaddr_in* sockStruct) {
	// Function to construct and return a working socket in either 'listen' or 'send' mode.
	SOCKET s;
	WSADATA wsaData;

	sockStruct->sin_family = AF_INET;
	sockStruct->sin_port = htons(port);
	sockStruct->sin_addr.s_addr = inet_addr(ipAddress);

	// Init winsockets
	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		printf("[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	// Create actual IPv4 TCP socket for listening
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("[ERR] Failed to create listening socket, error code %d\r\n", WSAGetLastError());
		exit(1);
	}

	if (mode == LISTEN) {
		// Bind listen socket
		int retcode = bind(s, (SOCKADDR*)sockStruct, sizeof(*sockStruct));
		int canListen = listen(s, 1);
	}

	return s;
}

int main(int argc, char* argv[]) {
	// Check for cmdline args
	char* flag = "";
	int noiseAmt = 0;
	int randomNoiseSeed = 0;

	// User input
	char userInputBuffer[5];
	int amtBitsFlipped = 0;

	// recv buffer
	char recvBuf[MSG_SIZE];

	// Basic user greeting and helptext
	printf("-------[NOISY CHANNEL]------- \r\n\r\n");

	// Deal with argv
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

	// Initialize sockets :)
	struct sockaddr_in sListen, sSend;
	SOCKET listenSocket;
	SOCKET transmitSocket;
	listenSocket = createSocket("127.0.0.1", LISTEN_PORT, LISTEN, &sListen);
	transmitSocket = createSocket("127.0.0.1", SEND_PORT, SEND, &sSend);

	while (1) {
		// Main action loop

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

		// Add noise to the buffered data here, depending on the flag provided by the user.
		if (!strcmp(flag, "-d")) {
			addDeterministicNoise(noiseAmt, recvBuf, recievedMessageSize, &amtBitsFlipped);
			printf("\r\nAdded deterministic noise!, flipped %d bits\r\n", amtBitsFlipped);
			printf("\r\n>After Noise: \r\n%s\r\n", recvBuf);
		}
		else if (!strcmp(flag, "-r")) {
			addRandomNoise(noiseAmt, randomNoiseSeed, recvBuf, recievedMessageSize, &amtBitsFlipped);
			printf("\r\nAdded random noise!, flipped %d bits\r\n", amtBitsFlipped);
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
