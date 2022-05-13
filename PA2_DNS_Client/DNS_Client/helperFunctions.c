#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#include "helperFunctions.h"


SOCKET createUdpSocket(char* ipAddress, int timeout) {
	// Function to construct and return a working socket in either 'listen' or 'send' mode.
	SOCKET s;
	WSADATA wsaData;
	struct sockaddr_in sockStruct;

	sockStruct.sin_family = AF_INET;
	sockStruct.sin_port = htons(DNS_PORT);
	sockStruct.sin_addr.s_addr = inet_addr(ipAddress);

	// Init winsockets
	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		fprintf(stderr, "[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	// Create actual IPv4 TCP socket for listening
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "[ERR] Failed to create listening socket, error code %d\r\n", WSAGetLastError());
		exit(1);
	}

	// Set timeout
	struct timeval timevalue;
	timevalue.tv_sec = timeout;
	timevalue.tv_usec = 0;
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timevalue, sizeof(timevalue)) < 0) {
		fprintf(stderr, "[ERR] Failed to set timeout!");
	}

	return s;
}

struct hostent* dnsQuery(char* domain) {

}