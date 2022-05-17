#include <stdio.h>
#include <stdlib.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library
#include <Ws2tcpip.h>

#include "Networking.h"

// Constants and general use variables
int wsaInitialized = 0;
WSADATA wsaData;
SOCKET sendSocket;

struct sockaddr_in netSocket;
int netSocketSize = sizeof(netSocket);

char recvBuffer[BUFFER_SIZE];
unsigned int recvBytes = 0;


void initUDP(char *serverIpAddr, unsigned int serverPort)
{
	int retval;
	// Try to init the winsocket thing
	if (!wsaInitialized)
	{
		retval = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (retval != 0)
			// TODO: Print error here
			exit(1);
		wsaInitialized = 1;
	}

	// Create our socket structure
	memset((void *)&netSocket, 0, sizeof(netSocket));
	netSocket.sin_family = AF_INET;
	netSocket.sin_addr.s_addr = inet_addr(serverIpAddr);
	netSocket.sin_port = htons(serverPort);

	//// Set timeout parameters
	//struct timeval timevalue;
	//timevalue.tv_sec = TIMEOUT_SECONDS;
	//timevalue.tv_usec = 0;
	

	// Create actual socket
	if ((sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("error: unable to initialize socket");
		return;
	}

	//// Push timeout to socket
	//if (setsockopt(sendSocket, SOL_SOCKET, SO_RCVTIMEO, &timevalue, sizeof(timevalue)) < 0) {
	//	perror("error: Failed to set timeout!");
	//}

	// Connect to the DNS server specified
	if (connect(sendSocket, (const struct sockaddr *)&netSocket, sizeof(netSocket)) != 0)
	{
		return;
	}

}

int sendUDP(char *buf, int bufferLength)
{
	int resp = sendto(sendSocket, buf, bufferLength, 0, (struct sockaddr*)&netSocket, sizeof(netSocket));
	return resp;
}

char* receiveUDP(int *recvBufferLength)
{
	int recvBits = recvfrom(sendSocket, recvBuffer, BUFFER_SIZE, 0, (struct sockaddr*) &netSocket, &netSocketSize);
	*recvBufferLength = recvBits;
	return recvBuffer;
}

void closeUDPSocket()
{
	closesocket(sendSocket);
}
