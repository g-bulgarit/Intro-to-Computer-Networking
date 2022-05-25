#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library
#include <Ws2tcpip.h>

#define DNS_SECTION_SIZE_QR		4
#define DNS_SECTION_SIZE_RR		8
#define	DNS_SECTION_SIZE_RR_FULL 10

typedef struct
{
	/*
									1  1  1  1  1  1
	  0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                      ID                       |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                    QDCOUNT                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                    ANCOUNT                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                    NSCOUNT                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	|                    ARCOUNT                    |
	+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	*/
	unsigned short id;
	unsigned short flags;   // entire 2nd line, containing QR, opcode, AA, TC, RD, RA, Z, R-code
	unsigned short qdcount; // question amount indicator
	unsigned short ancount; // answer amount indicator
	unsigned short nscount; // nameserver amount indicator
	unsigned short arcount; // records amount indicator 
} dnsHeader;

typedef struct
{
	/*
	                                1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                     QNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QTYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QCLASS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	*/
	char qname[1];				// This will contain the correctly formatted domain name, space will be allocated later
	unsigned short qtype;		// Query type, almost always 1 in our case
	unsigned short qclass;		// Question class, almost always 1 in our case
} dnsQuestion;

typedef struct
{
	/*
	                                1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                                               /
    /                      NAME                     /
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     CLASS                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TTL                      |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                   RDLENGTH                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
    /                     RDATA                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	*/
	char name[1];			// This will contain the correctly formatted domain name, space will be allocated later!
	unsigned short type:16;
	unsigned short u_class:16;
	unsigned int ttl:32;
	unsigned short rdlength:16;
	unsigned rdata[1];		// This will contain the returned data, space will be allocated later!
} dnsResource;


typedef struct
{
	dnsHeader *head;
	unsigned int size;
	dnsQuestion *qdBase;
	unsigned int qdSize;
	char *anBase;
	unsigned int anSize;
	char *nsBase;
	unsigned int nsSize;
	char *arBase;
	unsigned int arSize;
} dnsPacket;


dnsPacket* createDnsPacket(char* domainName, int domainLength, int* messageId);
dnsPacket* parseDnsServerResponse(char* responseBuffer, int responseSize);
