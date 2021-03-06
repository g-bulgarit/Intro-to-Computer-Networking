#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "dnsInfra.h"
#include "Networking.h"
#include "validationUtils.h"

struct hostent* dnsQueryResult;

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
	unsigned char userText[MAX_DOMAIN_SECTION_LEGNTH];
	struct sockaddr_in parsedResult;

	while (1) {
		int validDomain = 1;
		// Initialize new socket
		initUDP(dnsServerIP, UDP_PORT);

		// Deal with user
		printf("nsclient> ");
		scanf("%s", &userText);
		// Check if user wants to quit
		if (!strcmp(userText, "quit")) {
			exit(0);
		}

		// Check the domain - if the domain is invalid, quit
		if (checkDomain(userText) == 0) {
			printf("[ERROR] BAD NAME.\nWrong format for the given domain.\n");
			validDomain = 0;
		}

		// Query the DNS server
		if (validDomain) {
			dnsQueryResult = dnsQuery(userText);
		}

		// Parse response if successfull:
		if (successFlag && validDomain) {
			parsedResult.sin_addr = *(struct in_addr*)dnsQueryResult->h_addr_list;
			printf("%s\n", inet_ntoa(parsedResult.sin_addr));
		}

		// Close socket
		closeUDPSocket();
	}
	return 0;
}
