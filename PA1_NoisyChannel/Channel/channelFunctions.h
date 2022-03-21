#pragma once

// General Imports
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Winsock Imports
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") //Winsock Library

// Constants and other stuff
#define LISTEN_PORT		6000
#define SEND_PORT		6001
#define MSG_SIZE		500000
#define BYTE_SIZE_IN_BITS 8
#define HOSTNAME_LEN 300

// #define DEBUG

#define LISTEN			1
#define SEND			2

void addDeterministicNoise(int n, char* buffer, int bufSize, int* flippedBits);
void addRandomNoise(int prob, int seed, char* buffer, int bufferSize, int* flippedBits);
SOCKET createSocket(struct in_addr* ipAddress, int port, int mode, struct sockaddr_in* sockStruct, int* chosenPort);
struct in_addr getThisPcAddr();