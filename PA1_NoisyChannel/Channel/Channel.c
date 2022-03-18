// General Imports
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Winsock Imports
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") //Winsock Library

// Constants and other stuff
#define LISTEN_PORT		6000
#define SEND_PORT		6001
#define MSG_SIZE		500000
#define BYTE_SIZE_IN_BITS 8
#define HOSTNAME_LEN 300

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

SOCKET createSocket(struct in_addr* ipAddress, int port, int mode, struct sockaddr_in* sockStruct, int* chosenPort) {
	// Function to construct and return a working socket in either 'listen' or 'send' mode.
	SOCKET s;
	WSADATA wsaData;

	sockStruct->sin_family = AF_INET;
	sockStruct->sin_port = htons(port);
	sockStruct->sin_addr.s_addr = inet_addr(inet_ntoa(*ipAddress));

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
		if (retcode) {
			printf("[ERR] Failed to bind socket, perhaps the specified port is taken?");
			exit(1);
		}

		int canListen = listen(s, 1);
		if (canListen) {
			printf("[ERR] Failed to open a listening connection on the socket");
			exit(1);
		}
	}
	if (port == 0) {
		// Find out which port was automatically selected and push it into the chosenPort var
		int addrLength = sizeof(*sockStruct);
		getsockname(s, (struct SOCKADDR*)sockStruct, &addrLength);
		*chosenPort = ntohs(sockStruct->sin_port);
	}
	return s;
}

struct in_addr getThisPcAddr() {
	// Get this machines's IP address by looking up the hostname and then getting the IP from it.

	WSADATA wsaData;
	// Init winsockets
	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	char hostname[HOSTNAME_LEN];
	gethostname(hostname, HOSTNAME_LEN);

	struct hostent* parseIpAddr;
	parseIpAddr = gethostbyname(hostname);

	struct in_addr addr;
	addr.s_addr = *(u_long *)(parseIpAddr->h_addr_list[0]);
	return addr;
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
	struct sockaddr_in sReceiver;
	struct sockaddr_in sSender;
	struct in_addr localIP = getThisPcAddr();
	int senderPort = 0;
	int recieverPort = 0;
	

	while (1) {
		// Main action loop - create sockets for listening to the sender and reciever.
		SOCKET listenToSender =		createSocket(&localIP, 0, LISTEN, &sSender,		&senderPort);
		SOCKET listenToReceiver =	createSocket(&localIP, 0, LISTEN, &sReceiver,	&recieverPort);

		printf("[INFO] Started listening to the recieve socket, waiting for sender\r\n");
		printf("[INFO] This machine's IP is %s\r\n", inet_ntoa(localIP));
		printf("[INFO] \tSet sender target to IP:\t%s,\tport:%d\r\n", inet_ntoa(localIP), senderPort);
		printf("[INFO] \tSet reciever target to IP:\t%s,\tport:%d\r\n", inet_ntoa(localIP), recieverPort);
	#ifdef DEBUG
		printf("[DEBUG] Noise flag %s, amt %d, seed %d\r\n", flag, noiseAmt, randomNoiseSeed);
	#endif
		
		// Allocate (too much) memory for message
		char* recvBuf = (char*)malloc(sizeof(char) * MSG_SIZE);
		if (recvBuf == NULL) {
			printf("[ERR] Failed to allocate memory for recieving the message. Exiting");
			exit(1);
		}

		int senderParams = sizeof(struct sockaddr_in);
		int receiverParams = sizeof(struct sockaddr_in);
		SOCKET sockReciever = accept(listenToReceiver, (SOCKADDR*)&sReceiver, &receiverParams);
		SOCKET sockSender = accept(listenToSender, (SOCKADDR*)&sSender, &senderParams);
		printf("[INFO] Got two connections from Sender and Receiver\r\n");

		int recievedMessageSize = recv(sockSender, recvBuf, MSG_SIZE, 0);

		recvBuf = (char*)realloc(recvBuf, recievedMessageSize + 1);
		recvBuf[recievedMessageSize + 1] = '\0';

		if (recievedMessageSize) {
			printf("\r\n>: Recieved %d bytes: \r\n%s\r\n", recievedMessageSize, recvBuf);
		}
		closesocket(sockSender);

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


		// Connect to server socket
		/*int listen_retcode = connect(listenToReceiver, (SOCKADDR*)&sSend, sizeof(sSend));
		printf("[Success] Connected to reciever \r\n");*/
		int sentMessageSize = send(sockReciever, recvBuf, recievedMessageSize, 0);
		printf("[Success] Sent %d bytes\n", sentMessageSize);
		closesocket(sockReciever);

		printf(">: Continue? (yes/no)\r\n");
		amtBitsFlipped = 0;
		scanf("%s", &userInputBuffer);
		if (!strcmp(userInputBuffer, "no")) {
			exit(0);
		}
	}

	return 0;
}
