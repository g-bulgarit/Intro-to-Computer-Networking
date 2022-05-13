#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#include "helperFunctions.h"

struct in_addr getThisPcAddr() {
	// Get this machines's IP address by looking up the hostname and then getting the IP from it.

	WSADATA wsaData;
	// Init winsockets
	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		fprintf(stderr, "[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	char hostname[500];
	gethostname(hostname, 500);

	struct hostent* parseIpAddr;
	parseIpAddr = gethostbyname(hostname);

	struct in_addr addr;
	addr.s_addr = *(u_long *)(parseIpAddr->h_addr_list[0]);
	return addr;
}

int main() {
	char* placeholderIpAddr = "8.8.8.8"; // placeholder for cmd line args

	// 1. Get user input for IP of DNS server, default to 8.8.8.8 FROM COMMAND LINE!
	// 2. User-logic-loop:
	//		2.1 parse user inputs for domain names 
	//		2.2 do conversion to DNS request format
	//		2.3 send DNS request and recieve response
	//		2.4 parse response and print
	// 3. On user input 'quit', free memory and quit

	// Request constraints:
	// 1. UDP 53
	// 2. Timeout 2s

	// Definitions
	char userText[BUFSIZE];

	// Main loop: prompt user to interact
	while (1) {
		printf("nsclient> ");
		scanf("%s", &userText);

		// Check if user wants to quit
		if (!strcmp(userText, "quit"))
			exit(0);

		// Make DNS request
		struct sockaddr_in dnsServerSock;
		SOCKET s = createUdpSocket(placeholderIpAddr, TIMEOUT_SECONDS);

		// Construct msg here
		char* msg = "";

		// Send the message to the specified DNS server
		sendto(s, msg, sizeof(msg), 0, (struct sockaddr*)&dnsServerSock, sizeof(dnsServerSock));

		// Get reply
		char buffer[BUFSIZE];
		int n = recvfrom(s, buffer, BUFSIZE, 0, (struct sockaddr*)&dnsServerSock, sizeof(dnsServerSock));

		// Parse Reply
	
		printf("%s", buffer);
		return 0;
	}
}

