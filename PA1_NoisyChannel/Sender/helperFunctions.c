// General Imports
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// Winsock Imports
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library
#include "helperFunctions.h"


void setBit(char* buffer, int index, int value) {
	// Set specific bit in buffer to specific value
	buffer[index / BYTE_SIZE_IN_BITS] &= ~(1u << (7 - (index % BYTE_SIZE_IN_BITS))); // This sets the bit to be 0
	// Set the bit to 1 if needed
	if (value == 1) {
		buffer[index / BYTE_SIZE_IN_BITS] |= (value << (7 - (index % BYTE_SIZE_IN_BITS)));
	}
}

int getBit(char* buffer, int index) {
	// Get specific bit value from buffer using mask and shift
	int bit = (buffer[index / BYTE_SIZE_IN_BITS] >> (7 - (index % BYTE_SIZE_IN_BITS))) & 1;
	return bit;
}

void setControlBit(char* encodingBuffer, int blockOffset, int encodingOffset, int* bitPositions, int arrayLength, int bitToSet) {
	// Set hamming control-bits using an array of bits to check and a rolling xor, such that an even pairity is achieved.
	int controlBit = 0;
	for (int i = 0; i < arrayLength; i++)
	{
		controlBit ^= getBit(encodingBuffer, bitPositions[i] + encodingOffset); // was block offset
	}
	setBit(encodingBuffer, bitToSet + encodingOffset, controlBit);
#ifdef DEBUG
	printf("Set control bit %d to %d", bitToSet, controlBit);
#endif

}

SOCKET createSocket(char* ipAddress, int port, int mode, struct sockaddr_in* sockStruct) {
	// Function to construct and return a working socket in either 'listen' or 'send' mode.
	SOCKET s;
	WSADATA wsaData;

	sockStruct->sin_family = AF_INET;
	sockStruct->sin_port = htons(port);
	sockStruct->sin_addr.s_addr = inet_addr(ipAddress);

	// Init winsockets
	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		printf("[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	// Create actual IPv4 TCP socket for listening
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("[ERR] Failed to create listening socket, error code %d\r\n", WSAGetLastError());
		exit(1);
	}

	if (mode == LISTEN) {
		// Bind listen socket
		int retcode = bind(s, (SOCKADDR*)sockStruct, sizeof(*sockStruct));
		if (retcode) {
			printf("[ERR] Failed to bind socket, perhaps the specified port is taken?");
			exit(1);
		}

		int canListen = listen(s, 1);
		if (canListen) {
			printf("[ERR] Failed to open a listening connection on the socket");
			exit(1);
		}
	}
	return s;
}


void hamming(char* originalFileBuffer, char* encodedFileBuffer, int originalFileLength) {
	// Encode message with hamming code to allow single bit error correction or two-bit error detection.

	// Bits are numbered from 0 to 30 in each block.
	// The control bits are:
	//		0, 1, 3, 7, 15
	// The data bits are:
	//		2, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
	   
	int encodedBlockNumber = 0;
	for (int blockNumber = 0; blockNumber < (originalFileLength * BYTE_SIZE_IN_BITS); blockNumber += 26)
	{
		// Set data bits to correct places in the encoded file buffer
		setBit(encodedFileBuffer, 2 + encodedBlockNumber, getBit(originalFileBuffer, 0 + blockNumber));				// Original: 0		Encoded: 2

		setBit(encodedFileBuffer, 4 + encodedBlockNumber, getBit(originalFileBuffer, 1 + blockNumber));				// Original: 1		Encoded: 4
		setBit(encodedFileBuffer, 5 + encodedBlockNumber, getBit(originalFileBuffer, 2 + blockNumber));				// Original: 2		Encoded: 5
		setBit(encodedFileBuffer, 6 + encodedBlockNumber, getBit(originalFileBuffer, 3 + blockNumber));				// Original: 3		Encoded: 6

		setBit(encodedFileBuffer, 8 + encodedBlockNumber, getBit(originalFileBuffer, 4 + blockNumber));				// Original: 4		Encoded: 8
		setBit(encodedFileBuffer, 9 + encodedBlockNumber, getBit(originalFileBuffer, 5 + blockNumber));				// Original: 5		Encoded: 9
		setBit(encodedFileBuffer, 10 + encodedBlockNumber, getBit(originalFileBuffer, 6 + blockNumber));			// Original: 6		Encoded: 10
		setBit(encodedFileBuffer, 11 + encodedBlockNumber, getBit(originalFileBuffer, 7 + blockNumber));			// Original: 7		Encoded: 11
		setBit(encodedFileBuffer, 12 + encodedBlockNumber, getBit(originalFileBuffer, 8 + blockNumber));			// Original: 8		Encoded: 12
		setBit(encodedFileBuffer, 13 + encodedBlockNumber, getBit(originalFileBuffer, 9 + blockNumber));			// Original: 9		Encoded: 13
		setBit(encodedFileBuffer, 14 + encodedBlockNumber, getBit(originalFileBuffer, 10 + blockNumber));			// Original: 10		Encoded: 14

		setBit(encodedFileBuffer, 16 + encodedBlockNumber, getBit(originalFileBuffer, 11 + blockNumber));			// Original: 11		Encoded: 16
		setBit(encodedFileBuffer, 17 + encodedBlockNumber, getBit(originalFileBuffer, 12 + blockNumber));			// Original: 12		Encoded: 17
		setBit(encodedFileBuffer, 18 + encodedBlockNumber, getBit(originalFileBuffer, 13 + blockNumber));			// Original: 13		Encoded: 18
		setBit(encodedFileBuffer, 19 + encodedBlockNumber, getBit(originalFileBuffer, 14 + blockNumber));			// Original: 14		Encoded: 19
		setBit(encodedFileBuffer, 20 + encodedBlockNumber, getBit(originalFileBuffer, 15 + blockNumber));			// Original: 15		Encoded: 20
		setBit(encodedFileBuffer, 21 + encodedBlockNumber, getBit(originalFileBuffer, 16 + blockNumber));			// Original: 16		Encoded: 21
		setBit(encodedFileBuffer, 22 + encodedBlockNumber, getBit(originalFileBuffer, 17 + blockNumber));			// Original: 17		Encoded: 22
		setBit(encodedFileBuffer, 23 + encodedBlockNumber, getBit(originalFileBuffer, 18 + blockNumber));			// Original: 18		Encoded: 23
		setBit(encodedFileBuffer, 24 + encodedBlockNumber, getBit(originalFileBuffer, 19 + blockNumber));			// Original: 19		Encoded: 24
		setBit(encodedFileBuffer, 25 + encodedBlockNumber, getBit(originalFileBuffer, 20 + blockNumber));			// Original: 20		Encoded: 25
		setBit(encodedFileBuffer, 26 + encodedBlockNumber, getBit(originalFileBuffer, 21 + blockNumber));			// Original: 21		Encoded: 26
		setBit(encodedFileBuffer, 27 + encodedBlockNumber, getBit(originalFileBuffer, 22 + blockNumber));			// Original: 22		Encoded: 27
		setBit(encodedFileBuffer, 28 + encodedBlockNumber, getBit(originalFileBuffer, 23 + blockNumber));			// Original: 23		Encoded: 28
		setBit(encodedFileBuffer, 29 + encodedBlockNumber, getBit(originalFileBuffer, 24 + blockNumber));			// Original: 24		Encoded: 29
		setBit(encodedFileBuffer, 30 + encodedBlockNumber, getBit(originalFileBuffer, 25 + blockNumber));			// Original: 25		Encoded: 30

		// Calculate control bits (TODO):
		int controlBit0_Bits[] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 };
		int controlBit1_Bits[] = { 1, 2, 5, 6, 9, 10, 13, 14, 17, 18, 21, 22, 25, 26, 29, 30 };
		int controlBit3_Bits[] = { 3, 4, 5, 6, 11, 12, 13, 14, 19, 20, 21, 22, 27, 28, 29, 30 };
		int controlBit7_Bits[] = { 7, 8, 9, 10, 11, 12, 13, 14, 23, 24, 25, 26, 27, 28, 29, 30 };
		int controlBit15_Bits[] = { 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
#ifdef DEBUG
		printf("\r\nBefore:\r\n");
		for (int i = 0; i < 31; i++)
		{
			printf("%d", getBit(encodedFileBuffer, i + encodedBlockNumber));
		}
#endif
		setControlBit(encodedFileBuffer, blockNumber, encodedBlockNumber, controlBit0_Bits, 16, 0);								// Set 0-th control bit		(1)
		setControlBit(encodedFileBuffer, blockNumber, encodedBlockNumber, controlBit1_Bits, 16, 1);								// Set 1-st control bit		(2)
		setControlBit(encodedFileBuffer, blockNumber, encodedBlockNumber, controlBit3_Bits, 16, 3);								// Set 3-rd control bit		(4)
		setControlBit(encodedFileBuffer, blockNumber, encodedBlockNumber, controlBit7_Bits, 16, 7);								// Set 7-th control bit		(8)
		setControlBit(encodedFileBuffer, blockNumber, encodedBlockNumber, controlBit15_Bits, 16, 15);							// Set 15-th control bit	(16)
#ifdef DEBUG
		printf("\r\nAfter:\r\n");
		for (int i = 0; i < 31; i++)
		{
			printf("%d", getBit(encodedFileBuffer, i + encodedBlockNumber));
		}
#endif

#ifdef DEBUG
		printf("\r\n[!] Finished working on 26-bit block %d out of %d\r\n", blockNumber / 26, originalFileLength * BYTE_SIZE_IN_BITS / 26);
#endif
		encodedBlockNumber += 31;
	}

}
