#pragma once

// General Imports
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// Winsock Imports
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library

// General Constants
#define BYTE_SIZE_IN_BITS 8
#define MAX_FILENAME_LENGTH 3000
#define LISTEN			1
#define SEND			2

void setBit(char* buffer, int index, int value);
int getBit(char* buffer, int index);
void setControlBit(char* encodingBuffer, int blockOffset, int encodingOffset, int* bitPositions, int arrayLength, int bitToSet);
SOCKET createSocket(char* ipAddress, int port, int mode, struct sockaddr_in* sockStruct);
void hamming(char* originalFileBuffer, char* encodedFileBuffer, int originalFileLength);
