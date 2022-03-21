#pragma once
// General Imports
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// Winsock Imports
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define MSG_SIZE 500000
#define BYTE_SIZE_IN_BITS 8

#define LISTEN			1
#define SEND			2

// For development, remove before submitting TODO
// #define DEBUG

SOCKET createSocket(char* ipAddress, int port, int mode, struct sockaddr_in* sockStruct);
void unhamming(char* recievedMessageBuffer, char* decodedFileBuffer, int messageLength, int* fixedBits);