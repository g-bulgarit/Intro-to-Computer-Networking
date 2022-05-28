#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "dnsInfra.h"
#include "Networking.h"
#include "validationUtils.h"

int main(int argc, char* argv[])
{
	// Validate correct number of inputs
	if (argc != 2) {
		printf("[ERROR] BAD ARGS.\nIncorrect amount of arguments supplied\n Expected nsclient.exe <DNS server IP>. Exiting.\n");
		exit(-1);
	}

	// Get user's DNS server from argv[1] here and check validity.
	char* dnsServerIP = argv[1];

	if (ipValidate(dnsServerIP) == 0) {
		printf("[ERROR] BAD NAME.\nWrong format for the given IP address. Exiting.\n");
		exit(-1);
	}

	// Print header once
	printf("Enter a domain name below, or type 'quit' to exit.\n");
	unsigned char userText[255];
	struct sockaddr_in parsedResult;

	while (1) {
		// Initialize new socket
		initUDP("8.8.8.8", UDP_PORT);

		// Deal with user
		printf("nsclient> ");
		scanf("%s", &userText);
		// checkDomain(); // Do domain name validation here

		// Check if user wants to quit
		if (!strcmp(userText, "quit")) {
			exit(0);
		}

		// Parse response if successfull
		if (dnsQuery(userText)) {
			parsedResult.sin_addr = *(struct in_addr*)dnsQueryResult->h_addr_list[0];
			printf("%s \n", inet_ntoa(parsedResult.sin_addr));
		}

		// Close socket
		closeUDPSocket();
	}
	return 0;
}
