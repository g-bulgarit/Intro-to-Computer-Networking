#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library
#include <Ws2tcpip.h>


typedef struct
{
	unsigned int id;
	unsigned int flags;
	unsigned int qdcount; // question amt
	unsigned int ancount; // answer amt
	unsigned int nscount; // nameserver amt
	unsigned int arcount; // records amt 
} dnsHeader;

typedef struct
{
	char qname[1];
	unsigned int qtype;
	unsigned int qclass;
} dnsQuestion;

typedef struct
{
	char name[1];
	unsigned int type;
	unsigned int u_class;
	unsigned int ttl;
	unsigned int rdlength;
	unsigned rdata[1];
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
