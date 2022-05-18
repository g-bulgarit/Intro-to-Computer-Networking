#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library
#include <Ws2tcpip.h>


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
	unsigned int id;
	unsigned int flags;   // entire 2nd line, containing QR, opcode, AA, TC, RD, RA, Z, R-code
	unsigned int qdcount; // question amount indicator
	unsigned int ancount; // answer amount indicator
	unsigned int nscount; // nameserver amount indicator
	unsigned int arcount; // records amount indicator 
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
	char qname[1];			// This will contain the correctly formatted domain name, space will be allocated later
	unsigned int qtype;		// Query type, almost always 1 in our case
	unsigned int qclass;	// Question class, almost always 1 in our case
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
	unsigned int type;
	unsigned int u_class;
	unsigned int ttl;
	unsigned int rdlength;
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


dnsPacket* createDnsPacket(char* domainName, int domainLength);
