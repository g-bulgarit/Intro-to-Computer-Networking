#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#pragma once

#define DNS_PORT 53
#define TIMEOUT_SECONDS 2
#define BUFSIZE 500

SOCKET createUdpSocket(char* ipAddress, int timeout);
struct hostent* dnsQuery(char* domain);