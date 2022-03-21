#include "channelFunctions.h"

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
	int cap = pow(2, 16);
	srand(seed);
	for (int i = 0; i < bufferSize * BYTE_SIZE_IN_BITS; i++) {
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
		fprintf(stderr, "[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	// Create actual IPv4 TCP socket for listening
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "[ERR] Failed to create listening socket, error code %d\r\n", WSAGetLastError());
		exit(1);
	}

	if (mode == LISTEN) {
		// Bind listen socket
		int retcode = bind(s, (SOCKADDR*)sockStruct, sizeof(*sockStruct));
		if (retcode) {
			fprintf(stderr, "[ERR] Failed to bind socket, perhaps the specified port is taken?");
			exit(1);
		}

		int canListen = listen(s, 1);
		if (canListen) {
			fprintf(stderr, "[ERR] Failed to open a listening connection on the socket");
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
	if (wsaRes != NO_ERROR) {
		fprintf(stderr, "[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	char hostname[HOSTNAME_LEN];
	gethostname(hostname, HOSTNAME_LEN);

	struct hostent* parseIpAddr;
	parseIpAddr = gethostbyname(hostname);

	struct in_addr addr;
	addr.s_addr = *(u_long *)(parseIpAddr->h_addr_list[0]);
	return addr;
}