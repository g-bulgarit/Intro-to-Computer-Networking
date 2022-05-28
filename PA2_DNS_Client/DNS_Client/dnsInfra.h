#pragma once
//DNS Query Program
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//Header Files
#include "winsock2.h"
#include "windows.h"
#include "stdio.h"
#include <stdlib.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define MAX_DNS_REPLIES 20
#define DNS_BUF_SIZE 2048

//#define DEBUG

/*
	Note about struct ordering:
		We didn't manage to order the structs in the same way that the packet
		appears in the documentation. This is unfortunate because it would make
		for a much cleaner code package. Nevertheless - when we tried to order
		the struct by the correct order, the compiler optimization would make
		it not work out and the size not would be correct, causing the DNS server
		to misunderstand our request.
		The order used below is what we found online - it works, but it's not pretty.
*/

//DNS header structure
typedef struct s_dnsHeader
{
	/*	DNS header packet structure
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
	unsigned short id;			// ID number: to be incremented for consequitive mesasges.

	unsigned char rd : 1;		// recursion desired field
	unsigned char tc : 1;		// is message truncated
	unsigned char aa : 1;		// is the answer authoritive?
	unsigned char opcode : 4;	// opcode for type of message
	unsigned char qr : 1;		// opcode for query or reponse

	unsigned char rcode : 4;	// reponse code for the packet
	unsigned char cd : 1;		// check?
	unsigned char ad : 1;		// is the data authenticated
	unsigned char z : 1;		// does nothing, very relateable 
	unsigned char ra : 1;		// recursion available

	unsigned short q_count;		// amount of question records
	unsigned short ans_count;	// amount of answer records
	unsigned short auth_count;	// amount of authority records
	unsigned short add_count;	// amount of resource records
} dnsHeader;

//Constant sized fields of query structure
typedef struct s_dnsQuestion
{
	/*	Struct for the DNS question. QNAME is omitted
		as it will be filled directly from the DNS
		packet constructor.
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
	unsigned short qtype;
	unsigned short qclass;
} dnsQuestion;

//Constant sized fields of the resource record structure
#pragma pack(push, 1)
typedef struct s_resData
{
	/*	DNS Resource structure. NAME is omitted
		as it is taken care of in the parser.
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

	unsigned short type;
	unsigned short _class;
	unsigned int ttl;
	unsigned short data_len;
} resData;
#pragma pack(pop)

//Pointers to resource record contents
typedef struct s_resRecord
{
	unsigned char *name;
	struct s_resData *resourceStruct;
	unsigned char *resourceData;
} resRecord;

struct hostent* dnsQueryResult;

int dnsQuery(unsigned char *host);
void domainToDnsFormat(unsigned char*, unsigned char*);
unsigned char* dnsToDomainFormat(unsigned char*, unsigned char*, int*);
void setDnsQueryParams(dnsHeader* dns, int* idCounter);