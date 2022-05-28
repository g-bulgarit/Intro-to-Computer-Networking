#include "dnsInfra.h"
#include "Networking.h"

// Constants and general use variables

int wsaInitialized = 0;

int netSocketSize = sizeof(netSocket);

char recvBuffer[BUFFER_SIZE];
unsigned int recvBytes = 0;

void initUDP(char *serverIpAddr, unsigned int serverPort) {
	// Try to init the winsocket thing
	if (!wsaInitialized)
	{
		int retval = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (retval != 0)
			// TODO: Print error here
			exit(1);
		wsaInitialized = 1;
	}

	sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP packet for DNS queries

	//Configure the sockaddress structure with information of DNS server
	netSocket.sin_family = AF_INET;
	netSocket.sin_port = htons(serverPort);
	netSocket.sin_addr.s_addr = inet_addr(serverIpAddr);

	int timeoutMilliseconds = TIMEOUT_MILLISECONDS;
	if (setsockopt(sendSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeoutMilliseconds, sizeof(timeoutMilliseconds)) < 0) {
		perror("error: Failed to set timeout!");
	}
}

void closeUDPSocket()
{
	closesocket(sendSocket);
}