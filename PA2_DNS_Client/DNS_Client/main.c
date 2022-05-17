#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "Networking.h"
#define USER_BUFFER_SIZE 1024


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
	char userText[USER_BUFFER_SIZE];

	// Main loop: prompt user to interact
	while (1) {
		printf("nsclient> ");
		scanf("%s", &userText);

		// Check if user wants to quit
		if (!strcmp(userText, "quit"))
			exit(0);

		// TODO: Get DNS server IP from argv
		initUDP(placeholderIpAddr, 53);

		char* msg = "test";
		int sentBytes = sendUDP(msg, sizeof(msg));

		int receivedBytes = 0;
		receiveUDP(&receivedBytes);
		printf("\nReceived bytes: %d", receivedBytes);
		
	}
}
