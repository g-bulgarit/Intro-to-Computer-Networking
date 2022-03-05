#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define MSG_SIZE 500000
#define BYTE_SIZE_IN_BITS 8

// For development, remove before submitting TODO
#define DEBUG

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

void unhamming(char* recievedMessageBuffer, char* decodedFileBuffer, int messageLength) {
	// This function reverses the hamming code to retrieve the original message.

	// Bits are numbered from 0 to 30 in each block.
	// The control bits are:
	//		0, 1, 3, 7, 15
	// The data bits are:
	//		2, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
	int decodedBlockNumber = 0;
	for (int blockNumber = 0; blockNumber < messageLength * BYTE_SIZE_IN_BITS; blockNumber += 31)
	{
		// Set data bits to correct places in the encoded file buffer
		setBit(decodedFileBuffer, 0 + decodedBlockNumber, getBit(recievedMessageBuffer, 2 + blockNumber));			// Original: 0		Encoded: 2

		setBit(decodedFileBuffer, 1 + decodedBlockNumber, getBit(recievedMessageBuffer, 4 + blockNumber));			// Original: 1		Encoded: 4
		setBit(decodedFileBuffer, 2 + decodedBlockNumber, getBit(recievedMessageBuffer, 5 + blockNumber));			// Original: 2		Encoded: 5
		setBit(decodedFileBuffer, 3 + decodedBlockNumber, getBit(recievedMessageBuffer, 6 + blockNumber));			// Original: 3		Encoded: 6

		setBit(decodedFileBuffer, 4 + decodedBlockNumber, getBit(recievedMessageBuffer, 8 + blockNumber));			// Original: 4		Encoded: 8
		setBit(decodedFileBuffer, 5 + decodedBlockNumber, getBit(recievedMessageBuffer, 9 + blockNumber));			// Original: 5		Encoded: 9
		setBit(decodedFileBuffer, 6 + decodedBlockNumber, getBit(recievedMessageBuffer, 10 + blockNumber));			// Original: 6		Encoded: 10
		setBit(decodedFileBuffer, 7 + decodedBlockNumber, getBit(recievedMessageBuffer, 11 + blockNumber));			// Original: 7		Encoded: 11
		setBit(decodedFileBuffer, 8 + decodedBlockNumber, getBit(recievedMessageBuffer, 12 + blockNumber));			// Original: 8		Encoded: 12
		setBit(decodedFileBuffer, 9 + decodedBlockNumber, getBit(recievedMessageBuffer, 13 + blockNumber));			// Original: 9		Encoded: 13
		setBit(decodedFileBuffer, 10 + decodedBlockNumber, getBit(recievedMessageBuffer, 14 + blockNumber));		// Original: 10		Encoded: 14

		setBit(decodedFileBuffer, 11 + decodedBlockNumber, getBit(recievedMessageBuffer, 16 + blockNumber));		// Original: 11		Encoded: 16
		setBit(decodedFileBuffer, 12 + decodedBlockNumber, getBit(recievedMessageBuffer, 17 + blockNumber));		// Original: 12		Encoded: 17
		setBit(decodedFileBuffer, 13 + decodedBlockNumber, getBit(recievedMessageBuffer, 18 + blockNumber));		// Original: 13		Encoded: 18
		setBit(decodedFileBuffer, 14 + decodedBlockNumber, getBit(recievedMessageBuffer, 19 + blockNumber));		// Original: 14		Encoded: 19
		setBit(decodedFileBuffer, 15 + decodedBlockNumber, getBit(recievedMessageBuffer, 20 + blockNumber));		// Original: 15		Encoded: 20
		setBit(decodedFileBuffer, 16 + decodedBlockNumber, getBit(recievedMessageBuffer, 21 + blockNumber));		// Original: 16		Encoded: 21
		setBit(decodedFileBuffer, 17 + decodedBlockNumber, getBit(recievedMessageBuffer, 22 + blockNumber));		// Original: 17		Encoded: 22
		setBit(decodedFileBuffer, 18 + decodedBlockNumber, getBit(recievedMessageBuffer, 23 + blockNumber));		// Original: 18		Encoded: 23
		setBit(decodedFileBuffer, 19 + decodedBlockNumber, getBit(recievedMessageBuffer, 24 + blockNumber));		// Original: 19		Encoded: 24
		setBit(decodedFileBuffer, 20 + decodedBlockNumber, getBit(recievedMessageBuffer, 25 + blockNumber));		// Original: 20		Encoded: 25
		setBit(decodedFileBuffer, 21 + decodedBlockNumber, getBit(recievedMessageBuffer, 26 + blockNumber));		// Original: 21		Encoded: 26
		setBit(decodedFileBuffer, 22 + decodedBlockNumber, getBit(recievedMessageBuffer, 27 + blockNumber));		// Original: 22		Encoded: 27
		setBit(decodedFileBuffer, 23 + decodedBlockNumber, getBit(recievedMessageBuffer, 28 + blockNumber));		// Original: 23		Encoded: 28
		setBit(decodedFileBuffer, 24 + decodedBlockNumber, getBit(recievedMessageBuffer, 29 + blockNumber));		// Original: 24		Encoded: 29
		setBit(decodedFileBuffer, 25 + decodedBlockNumber, getBit(recievedMessageBuffer, 30 + blockNumber));		// Original: 25		Encoded: 30
#ifdef DEBUG
		printf("[!] Finished working on block %d out of %d\r\n", blockNumber / 31, messageLength * BYTE_SIZE_IN_BITS / 31);
#endif
		decodedBlockNumber += 26;
	}

}


int main(int argc, char* argv[]) {
	FILE* wfp;
	char fileNameBuffer[3000];

	printf("-------[RECIEVER]------- \r\n\r\n");
	// Check for cmdline args
	if (argc != 3) {
		printf("[ERR] Please provide all required command-line arguments.\r\n Reciever.exe <ip_address> <port>\r\n");
		exit(1);
	}

	// Set up connection details
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[2]));
	server.sin_addr.s_addr = inet_addr(argv[1]);

	struct sockaddr_in client;

	// initialize windows networking and check for errors.
	WSADATA wsaData;
	SOCKET rxSocket;

	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		printf("[ERR] WSAStartup() failed.\r\n");
		exit(1);
	}

	// Create actual IPv4 TCP socket
	if ((rxSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("[ERR] Failed to create socket, error code %d\r\n", WSAGetLastError());
		exit(1);
	}

	// Bind socket
	int retcode = bind(rxSocket, (SOCKADDR*)&server, sizeof(server));
	int listen_retcode = listen(rxSocket, 1);
	// TODO: Test retcodes

	// Setup recv buffer
	char* recvBuf = (char*)malloc(sizeof(char) * MSG_SIZE);
	printf("[Start] Started socket, listening to connections from the noisy channel\r\n");

	while (1) {
		int c = sizeof(struct sockaddr_in);
		SOCKET s = accept(rxSocket, (SOCKADDR*)&client, &c);
		int recievedMessageSize = recv(s, recvBuf, MSG_SIZE, 0);
		recvBuf = (char*)realloc(recvBuf, recievedMessageSize + 1);
		recvBuf[recievedMessageSize + 1] = '\0';

		if (recievedMessageSize) {
			printf("[Success] Recieved %d bytes\r\n", recievedMessageSize);
			printf("[Success] Recieved:\r\n%s\r\n", recvBuf);
		}

		// Decode the file here
		int decodedFileSize = recievedMessageSize * (26.0 / 31.0);
		char* decodedFileBuffer = (char*)malloc(sizeof(char) * decodedFileSize + 1);
		if (decodedFileBuffer == NULL) {
			exit(1);
			// todo print
		}
		// Set entire buffer to 0.
		memset(decodedFileBuffer, 0, sizeof(char) * decodedFileSize + 1);

		unhamming(recvBuf, decodedFileBuffer, recievedMessageSize);
		decodedFileBuffer[decodedFileSize + 1] = '\0';
		printf("[Decode] Decoded %d bytes\r\n", decodedFileSize);
		printf("[Decode] Decoded:\r\n%s\r\n", decodedFileBuffer);

		// Save the file with the help of some user input
		printf(">: Enter filename: ");
		scanf("%s", &fileNameBuffer);
		wfp = fopen(fileNameBuffer, "wb+");
		fwrite(decodedFileBuffer, 1, decodedFileSize, wfp);
		fclose(wfp);
		closesocket(s);
	}

	return 0;
}
