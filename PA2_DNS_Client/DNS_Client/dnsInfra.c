#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include "dnsInfra.h"


void splitDomainName(char *domain, char* outputString)
{
	// Convert domain name to DNS format

	const char delimeter = '.';
	char* substring;
	char* copiedDomainBuffer;
	unsigned int outStrPosition = 0;
	int strLength;

	copiedDomainBuffer = (char*)calloc((strlen(domain) + 1), 1); // Increase copied domain container size
	strcpy(copiedDomainBuffer, domain);

	// Use str-token to split the string and append the size of each block to the start of each block
	substring = strtok(copiedDomainBuffer, &delimeter);
	while (substring != NULL)
	{
		// Calculate size of substring and append it before the substring itself.
		strLength = strlen(substring);
		outputString[outStrPosition] = (char)strLength;
		memcpy(&outputString[outStrPosition + 1], substring, strLength); // Append substring
		outStrPosition += (strLength + 1);

		// Keep splitting the string from where we stopped
		substring = strtok(NULL, &delimeter);
	}

	outputString[outStrPosition] = '\0';
	free(copiedDomainBuffer);
}

int addDnsQueryToMsg(dnsPacket *dns, char *qdomain, unsigned int qdomainSize, unsigned int qtype, unsigned int qclass)
{
	// Take the DNS packet and:
	//	1. Allocate space for the domain query
	//  2. Reformat the domain in the format specified by the DNS protocol
	//  3. Add it to the packet

	dnsQuestion* qdBase;
	unsigned int qdSize;
	unsigned int qnameActualSize;
	unsigned int qnamePreviousSize;

	// find the size of this question block
	qnameActualSize = qdomainSize;
	qdSize = sizeof(dnsQuestion) + qnameActualSize;

	// TODO: malloc check
	qdBase = (dnsQuestion*)malloc(sizeof(dnsQuestion));

	// Set query type and query class as given from outside
	qdBase->qtype = htons(qtype);
	qdBase->qclass = htons(qclass);

	// Unpack previous value and push in new value
	qnamePreviousSize = dns->size;
	dns->size += qdSize;

	// Add size to the header block to fit both itself and the query
	dns->head = (dnsHeader*)realloc(dns->head, dns->size);
	if (dns->head == NULL) {
		// TODO: Handle failed malloc here
		return -1;
	}

	// Set pointer to the query part of the packet
	qdBase = (dnsQuestion *)((int)(dns->head) + sizeof(dnsHeader));
	dns->qdBase = qdBase;
	dns->qdSize += qdSize;

	// If the dns packet is already built, move everything forward to have space for the domain
	memcpy((void *)((DWORD_PTR)(&qdBase->qtype) + (DWORD_PTR)(qnameActualSize)), &qdBase->qtype, (sizeof(qdBase->qtype) + sizeof(qdBase->qclass)));

	// split domain name to fit DNS protocal specification
	splitDomainName(qdomain, qdBase->qname);

	return ++dns->head->qdcount;
}

void dnsHostToNetwork(dnsPacket *dns)
{
	// Switch from host to network order
	dns->head->id = htons(dns->head->id);
	dns->head->flags = htons(dns->head->flags);
	dns->head->qdcount = htons(dns->head->qdcount);
	dns->head->ancount = htons(dns->head->ancount);
	dns->head->arcount = htons(dns->head->arcount);
	dns->head->nscount = htons(dns->head->nscount);
}


dnsPacket* createDnsPacket(char* domainName, int domainLength)
dnsPacket* createDnsPacket(char* domainName, int domainLength, int* messageId)
{
	// Build the actual packet
	dnsPacket* dnsPack;
	dnsHeader* dnsHead;
	unsigned int headerSize = sizeof(dnsHeader);

	dnsPack = (dnsPacket*)calloc(1, sizeof(dnsPacket));
	dnsHead = (dnsHeader*)calloc(1, headerSize);

	// Initially, the DNS packet size is just the header
	dnsPack->head = dnsHead;
	dnsPack->size = headerSize;

	dnsPack->head->flags = 0x0100;  // Use RD flag (recursion desired)

	// set the required header fields
	dnsPack->head->id = *messageId;
	(*messageId)++; // Increment the mesasge ID for next time.

	// Add query
	addDnsQueryToMsg(dnsPack, domainName, domainLength, 1, 1);

	// Switch H2N order
	dnsHostToNetwork(dnsPack);

	return dnsPack;
}