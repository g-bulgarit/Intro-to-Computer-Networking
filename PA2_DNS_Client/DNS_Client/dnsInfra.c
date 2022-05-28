#define _CRT_SECURE_NO_WARNINGS
#include "dnsInfra.h"
#include "Networking.h"

/*
	Full infrastructure for constructing and deconstructing a DNS packet.
*/

// Global parameter for incrementing the message ID for the DNS packets
int globalMessageId = 0;

struct hostent* dnsQuery(unsigned char *domainName)
{
	// Function to create a DNS packet according to the RFC specification and use it
	// to query a DNS server and parse it's response.
	unsigned char buf[DNS_BUF_SIZE];
	unsigned char* dnsDomainName;
	unsigned char* readPtr;

	int sizeSocket = 0;
	int i, j, stoppingPtr;

	successFlag = 1;

	struct hostent* dnsResponse = (struct hostent*)malloc(sizeof(struct hostent));
	char **foundIpAddrList[2][MAX_DNS_REPLIES] = { 0 };

	resRecord dnsReponses[MAX_DNS_REPLIES];
	dnsHeader *dns = NULL;
	dnsQuestion *qinfo = NULL;

	// Set the DNS structure to standard queries and fill known fields
	dns = (dnsHeader*)&buf;
	setDnsQueryParams(dns, &globalMessageId);

	// Get pointer to the query portion and construct the query
	dnsDomainName = (unsigned char*)&buf[sizeof(dnsHeader)];
	domainToDnsFormat(dnsDomainName, domainName);
	qinfo = (dnsQuestion*)&buf[sizeof(dnsHeader) + (strlen((const char*)dnsDomainName) + 1)];
	qinfo->qtype = htons(1);
	qinfo->qclass = htons(1);

	// Send DNS packet to the specified DNS server
	if (sendto(sendSocket, (char*)buf, sizeof(dnsHeader) + (strlen((const char*)dnsDomainName) + 1) + sizeof(dnsQuestion), 0, (struct sockaddr*)&netSocket, sizeof(netSocket)) == SOCKET_ERROR)
	{
		printf("[ERROR] Failed to send.\nError Code: %d\n", WSAGetLastError());
		successFlag = 0;
	}

	sizeSocket = sizeof(netSocket);

	// Receive DNS response - throw errors for timeout and other possible stuff
	if (recvfrom(sendSocket, (char*)buf, DNS_BUF_SIZE, 0, (struct sockaddr*)&netSocket, &sizeSocket) == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode == 10060) {
			// Throw specific error for timeout
			printf("[ERROR] Request timed out after %d seconds - Error Code: %d\n", TIMEOUT_MILLISECONDS / 1000, errCode);
			successFlag = 0;
		}
		else {
			// Throw general error
			printf("[ERROR] Failed to receive.\nError Code: %d\n", errCode);
			successFlag = 0;
		}
	}

	// Read received buffer into a DNS packet
	dns = (dnsHeader*)buf;

	// Skip to the replies from the DNS server with a pointer
	readPtr = &buf[sizeof(dnsHeader) + (strlen((const char*)dnsDomainName) + 1) + sizeof(dnsQuestion)];
	stoppingPtr = 0;

	// Check if we didnt get an answer:
	if (ntohs(dns->ans_count) == 0) {
		printf("[ERROR] NONEXISTENT\n");
		successFlag = 0;
	}

	if (successFlag == 1)
	{
		// Start going over all answers
		for (i = 0; i < ntohs(dns->ans_count); i++)
		{
			// For each answer in the DNS response,
			dnsReponses[i].name = dnsToDomainFormat(readPtr, buf, &stoppingPtr);	// Convert name to DNS packet format
			readPtr += stoppingPtr;

			// Add to resource
			dnsReponses[i].resourceStruct = (resData*)(readPtr);
			readPtr += sizeof(resData);

			if (ntohs(dnsReponses[i].resourceStruct->type) == 1 && ntohs(dnsReponses[i].resourceStruct->_class) == 1) // Look at *responses* only
			{
				// Allocate room for the resource struct
				dnsReponses[i].resourceData = (unsigned char*)malloc(ntohs(dnsReponses[i].resourceStruct->data_len));
				for (j = 0; j < ntohs(dnsReponses[i].resourceStruct->data_len); j++)
					dnsReponses[i].resourceData[j] = readPtr[j];

				// Add null byte to terminate the reposnse string
				dnsReponses[i].resourceData[ntohs(dnsReponses[i].resourceStruct->data_len)] = '\0';
				readPtr += ntohs(dnsReponses[i].resourceStruct->data_len);
			}
			else // Skip
			{
				dnsReponses[i].resourceStruct = dnsToDomainFormat(readPtr, buf, &stoppingPtr);
				readPtr += stoppingPtr;
			}
		}

		// Construct a list of a IP addresses
		int addrAmt = 0;
		for (i = 0; i < ntohs(dns->ans_count); i++)
		{
			if (ntohs(dnsReponses[i].resourceStruct->type) == 1) // Look at *responses* only
			{
				long *ipAddrPtr;
				ipAddrPtr = (long*)dnsReponses[i].resourceData;
				foundIpAddrList[0][addrAmt] = ipAddrPtr;
				addrAmt++;
			}
		}

#ifdef DEBUG
		printf("[DEBUG] answers: %d\n", ntohs(dns->ans_count));
#endif

		// Fill the hostent struct:
		//	Regarding name and aliases, we just put the original domain name
		//	and assummed IPv4 address only in the addrtype field.
		//	Everything else matches the hostent struct.

		dnsResponse->h_name = domainName;
		dnsResponse->h_aliases = domainName;
		dnsResponse->h_addrtype = AF_INET;
		dnsResponse->h_length = 4;
		dnsResponse->h_addr_list = foundIpAddrList;
	}
	return dnsResponse;

}

void setDnsQueryParams(dnsHeader* dns, int* idCounter) {
	// Set all required DNS header paramters, assuming we always want to query.

	int currentId = (*idCounter)++;
	dns->id = currentId;
	dns->qr = 0;				// message type is query
	dns->opcode = 0;			// normal query
	dns->aa = 0;				// non authoritive
	dns->tc = 0;				// full message
	dns->rd = 1;				// use recursion
	dns->ra = 0;				// not available
	dns->z = 0;
	dns->ad = 0;
	dns->cd = 0;
	dns->rcode = 0;
	dns->q_count = htons(1);	// Always 1 query
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = 0;
}

unsigned char* dnsToDomainFormat(unsigned char* reader, unsigned char* buffer, int* count)
{
	// Un-compress the DNS name format to a humean readable format
	// from:	3www4test3com
	// to:		www.test.com

	unsigned char *domainName;
	unsigned int p = 0, jumped = 0, offset;

	// Looping variables:
	int i, j;

	*count = 1;

	// Temporary domain container:
	domainName = (unsigned char*)malloc(256);
	domainName[0] = '\0';

	while (*reader != 0) {
		if (*reader >= 192) {
			offset = (*reader) * 256 + *(reader + 1) - 0xC000;
			reader = buffer + offset - 1;
			jumped = 1;
		}
		else {
			domainName[p++] = *reader;
		}
		reader++;

		if (jumped == 0) *count = *count + 1; // Keep counting
	}

	domainName[p] = '\0'; // Close string

	if (jumped == 1)
	{
		*count = *count + 1; //number of steps we actually moved forward in the packet
	}

	// This is the reverse of domainToDnsFormat - Convert to actual domain.
	for (i = 0; i < (int)strlen((const char*)domainName); i++) {
		p = domainName[i];
		for (j = 0; j < (int)p; j++) {
			domainName[i] = domainName[i + 1];
			i = i + 1;
		}
		domainName[i] = '.';
	}
	domainName[i - 1] = '\0'; // Close string and return
	return domainName;
}


void domainToDnsFormat(unsigned char* outQname, unsigned char* domain)
{
	// Convert domain name to the format specified in the RFC,
	// namely:
	// www.abcde.com -> 3www5abcde3com

	strcat((char*)domain, "."); // Housekeeping - need a dot in the end to make algo easy to write...
	int previousStop = 0, i;

	for (i = 0; i < (int)strlen((char*)domain); i++) {
		if (domain[i] == '.') {
			*outQname++ = i - previousStop;
			for (; previousStop < i; previousStop++) {
				*outQname++ = domain[previousStop]; // Set character to number
			}
			previousStop++;
		}
	}
	// Finish by closing the string
	*outQname++ = '\0';
}
