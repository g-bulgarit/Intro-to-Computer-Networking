#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

#include "Networking.h"
#include "dnsInfra.h"
#define USER_BUFFER_SIZE 1024


int main() {
	char* placeholderIpAddr = "62.219.186.7"; // placeholder for cmd line args

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
	char userText[USER_BUFFER_SIZE];
	int messageId = 0xa123;
	int receivedBytes;

	dnsPacket* dnsMessage;
	dnsPacket* dnsResponsePacket;

	// Main loop: prompt user to interact
	while (1) {
		printf("nsclient> ");
		scanf("%s", &userText);

		// Check if user wants to quit
		if (!strcmp(userText, "quit"))
			exit(0);

		// Create DNS packet
		dnsMessage = createDnsPacket(userText, strlen(userText), &messageId);

		// TODO: Get DNS server IP from argv
		initUDP(placeholderIpAddr, 53);

		// ...and send it
		int sentBytes = sendUDP((char*)dnsMessage->head, dnsMessage->size);

		// Listen for DNS result
		
		char* dnsResult = receiveUDP(&receivedBytes);
		printf("\nReceived bytes: %d\n", receivedBytes);

		// Parse the result (TODO)
	}
}
