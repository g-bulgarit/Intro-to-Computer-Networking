#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library


void setBit(char* buffer, int bufferSize, int index, int value) {
	// Set specific bit in buffer

	buffer[index / bufferSize] &= ~(1u << (7 - (index % bufferSize))); // This sets the bit to be 0
	// Set the bit to 1 if needed
	if (value == 1) {
		buffer[index / bufferSize] |= (value << (7 - (index % bufferSize)));
	}
}

int getBit(char* buffer, int bufferSize, int index) {
	// Set specific bit from buffer
	int bit = (buffer[index / bufferSize] >> (7 - (index % bufferSize))) & 1;

	//int bit = buffer[index / bufferSize] &= ~(1u << (8 - (index % bufferSize)));
	return bit;

}

void hamming(char* originalFileBuffer, char* encodedFileBuffer, int originalFileLength) {
	// Bits are numbered from 0 to 30 in each block.
	// The control bits are:
	//		0, 1, 3, 7, 15
	// The data bits are:
	//		2, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30

	int encodedBlockNumber = 0;
	for (int blockNumber = 0; blockNumber < originalFileLength * 8; blockNumber += 26)
	{
		// Set data bits to correct places in the encoded file buffer
		setBit(encodedFileBuffer, 8, 2 + encodedBlockNumber, getBit(originalFileBuffer, 8, 0 + blockNumber));				// Original: 0		Encoded: 2

		setBit(encodedFileBuffer, 8, 4 + encodedBlockNumber, getBit(originalFileBuffer, 8, 1 + blockNumber));				// Original: 1		Encoded: 4
		setBit(encodedFileBuffer, 8, 5 + encodedBlockNumber, getBit(originalFileBuffer, 8, 2 + blockNumber));				// Original: 2		Encoded: 5
		setBit(encodedFileBuffer, 8, 6 + encodedBlockNumber, getBit(originalFileBuffer, 8, 3 + blockNumber));				// Original: 3		Encoded: 6

		setBit(encodedFileBuffer, 8, 8 + encodedBlockNumber, getBit(originalFileBuffer, 8, 4 + blockNumber));				// Original: 4		Encoded: 8
		setBit(encodedFileBuffer, 8, 9 + encodedBlockNumber, getBit(originalFileBuffer, 8, 5 + blockNumber));				// Original: 5		Encoded: 9
		setBit(encodedFileBuffer, 8, 10 + encodedBlockNumber, getBit(originalFileBuffer, 8, 6 + blockNumber));				// Original: 6		Encoded: 10
		setBit(encodedFileBuffer, 8, 11 + encodedBlockNumber, getBit(originalFileBuffer, 8, 7 + blockNumber));				// Original: 7		Encoded: 11
		setBit(encodedFileBuffer, 8, 12 + encodedBlockNumber, getBit(originalFileBuffer, 8, 8 + blockNumber));				// Original: 8		Encoded: 12
		setBit(encodedFileBuffer, 8, 13 + encodedBlockNumber, getBit(originalFileBuffer, 8, 9 + blockNumber));				// Original: 9		Encoded: 13
		setBit(encodedFileBuffer, 8, 14 + encodedBlockNumber, getBit(originalFileBuffer, 8, 10 + blockNumber));			// Original: 10		Encoded: 14

		setBit(encodedFileBuffer, 8, 16 + encodedBlockNumber, getBit(originalFileBuffer, 8, 11 + blockNumber));			// Original: 11		Encoded: 16
		setBit(encodedFileBuffer, 8, 17 + encodedBlockNumber, getBit(originalFileBuffer, 8, 12 + blockNumber));			// Original: 12		Encoded: 17
		setBit(encodedFileBuffer, 8, 18 + encodedBlockNumber, getBit(originalFileBuffer, 8, 13 + blockNumber));			// Original: 13		Encoded: 18
		setBit(encodedFileBuffer, 8, 19 + encodedBlockNumber, getBit(originalFileBuffer, 8, 14 + blockNumber));			// Original: 14		Encoded: 19
		setBit(encodedFileBuffer, 8, 20 + encodedBlockNumber, getBit(originalFileBuffer, 8, 15 + blockNumber));			// Original: 15		Encoded: 20
		setBit(encodedFileBuffer, 8, 21 + encodedBlockNumber, getBit(originalFileBuffer, 8, 16 + blockNumber));			// Original: 16		Encoded: 21
		setBit(encodedFileBuffer, 8, 22 + encodedBlockNumber, getBit(originalFileBuffer, 8, 17 + blockNumber));			// Original: 17		Encoded: 22
		setBit(encodedFileBuffer, 8, 23 + encodedBlockNumber, getBit(originalFileBuffer, 8, 18 + blockNumber));			// Original: 18		Encoded: 23
		setBit(encodedFileBuffer, 8, 24 + encodedBlockNumber, getBit(originalFileBuffer, 8, 19 + blockNumber));			// Original: 19		Encoded: 24
		setBit(encodedFileBuffer, 8, 25 + encodedBlockNumber, getBit(originalFileBuffer, 8, 20 + blockNumber));			// Original: 20		Encoded: 25
		setBit(encodedFileBuffer, 8, 26 + encodedBlockNumber, getBit(originalFileBuffer, 8, 21 + blockNumber));			// Original: 21		Encoded: 26
		setBit(encodedFileBuffer, 8, 27 + encodedBlockNumber, getBit(originalFileBuffer, 8, 22 + blockNumber));			// Original: 22		Encoded: 27
		setBit(encodedFileBuffer, 8, 28 + encodedBlockNumber, getBit(originalFileBuffer, 8, 23 + blockNumber));			// Original: 23		Encoded: 28
		setBit(encodedFileBuffer, 8, 29 + encodedBlockNumber, getBit(originalFileBuffer, 8, 24 + blockNumber));			// Original: 24		Encoded: 29
		setBit(encodedFileBuffer, 8, 30 + encodedBlockNumber, getBit(originalFileBuffer, 8, 25 + blockNumber));			// Original: 25		Encoded: 30

		// Calculate control bits:
		int controlBit0 = 1;
		int controlBit1 = 1;
		int controlBit2 = 1;
		int controlBit3 = 1;
		int controlBit4 = 1;

		// Set control bits
		setBit(encodedFileBuffer, 8, 0 + encodedBlockNumber, controlBit0);			// Original: CB0	Encoded: 0
		setBit(encodedFileBuffer, 8, 1 + encodedBlockNumber, controlBit1);			// Original: CB1	Encoded: 1
		setBit(encodedFileBuffer, 8, 3 + encodedBlockNumber, controlBit2);			// Original: CB2	Encoded: 3
		setBit(encodedFileBuffer, 8, 7 + encodedBlockNumber, controlBit3);			// Original: CB3	Encoded: 7
		setBit(encodedFileBuffer, 8, 15 + encodedBlockNumber, controlBit4);		// Original: CB4	Encoded: 15
		printf("[!] Finished working on block %d out of %d\r\n", blockNumber / 26, originalFileLength / 26);
		encodedBlockNumber += 31;
	}

}

int main(int argc, char* argv[]) {
	int stopUserInput = 0;
	char fileNameBuffer[3000];

	char* fileContentBuffer;

	// Test
	int buf = 0xFAFA;
	printf("%d", getBit(&buf, 8, 0));
	printf("%d", getBit(&buf, 8, 1));
	printf("%d", getBit(&buf, 8, 2));
	printf("%d", getBit(&buf, 8, 3));
	printf("%d", getBit(&buf, 8, 4));
	printf("%d", getBit(&buf, 8, 5));
	printf("%d", getBit(&buf, 8, 6));
	printf("%d", getBit(&buf, 8, 7));
	printf("%d", getBit(&buf, 8, 8));
	printf("%d", getBit(&buf, 8, 9));
	printf("%d", getBit(&buf, 8, 10));
	printf("%d", getBit(&buf, 8, 11));
	printf("%d", getBit(&buf, 8, 12));
	printf("%d", getBit(&buf, 8, 13));
	printf("%d", getBit(&buf, 8, 14));
	printf("%d", getBit(&buf, 8, 15));

	printf("\r\n");
	setBit(&buf, 8, 0, 0);
	setBit(&buf, 8, 1, 0);
	setBit(&buf, 8, 2, 0);
	setBit(&buf, 8, 3, 0);
	setBit(&buf, 8, 4, 1);
	setBit(&buf, 8, 5, 1);
	setBit(&buf, 8, 6, 1);
	setBit(&buf, 8, 7, 1);

	printf("%d", getBit(&buf, 8, 0));
	printf("%d", getBit(&buf, 8, 1));
	printf("%d", getBit(&buf, 8, 2));
	printf("%d", getBit(&buf, 8, 3));
	printf("%d", getBit(&buf, 8, 4));
	printf("%d", getBit(&buf, 8, 5));
	printf("%d", getBit(&buf, 8, 6));
	printf("%d", getBit(&buf, 8, 7));


	


	FILE* rfp;

#ifdef DEBUG
	printf("-------[SENDER]------- \r\n\r\n");
#endif

	// Check for cmdline args
	if (argc != 3) {
		printf("[ERR] Please provide all required command-line arguments.\r\n. Sender.exe <ip_address> <port>\r\n");
		exit(1);
	}

	// Set up connection details
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(atoi(argv[2]));
	remote.sin_addr.s_addr = inet_addr(argv[1]);

	#ifdef DEBUG
	printf("[DEBUG] Starting sender with IP %s and port %d\r\n", argv[1], remote.sin_port);
	#endif

	// initialize windows networking and check for errors.
	WSADATA wsaData;
	SOCKET txSocket;

	int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaRes != NO_ERROR) {
		printf("[ERR] WSAStartup() failed.\n");
		exit(1);
	}


	while (!stopUserInput) {
		// Create actual IPv4 TCP socket
		if ((txSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			printf("[ERR] Failed to create socket, error code %d", WSAGetLastError());
			exit(1);
		}
		// Connect to server socket
		int listen_retcode = connect(txSocket, (SOCKADDR*)&remote, sizeof(remote));

		#ifdef DEBUG
			printf("[Start] Connected to the Noisy Channel\r\n");
		#endif
		printf(">: Enter file name: ");
		scanf("%s", &fileNameBuffer);

		if (!strcmp(fileNameBuffer, "quit"))
			exit(0);
		
		// Check if the file exists and open it.
		rfp = fopen(fileNameBuffer, "rb");

		// Get file size: taken partly from https://codereview.stackexchange.com/questions/112613/memory-safe-file-reading-in-c
		fseek(rfp, 0, SEEK_END); // get offset at end of file
		int fileSize = ftell(rfp) + 1; // Get size
		rewind(rfp, 0, SEEK_SET); // seek back to beginning
		fileContentBuffer = (char*)malloc(sizeof(char) * (fileSize)); // allocate enough memory in bytes.



		if (rfp != NULL) {
			size_t newLen = fread(fileContentBuffer, sizeof(char), fileSize, rfp);
			if (ferror(rfp) != 0) {
				printf("[ERR] Error reading input file, make sure the path is correct. Exiting.");
				exit(1);
			}
			else {
				fileContentBuffer[newLen++] = '\0';
			}
			fclose(rfp);
		}
		else {
			printf("[ERR] Failed to allocate enough memory for reading the file.\r\nExiting.");
			exit(1);
		}
		
		// Encode with hamming(31,26,3)
		int encodedFileSize = (int) (fileSize - 1) * (31.0 / 26.0);
		char* encodedFileBuffer = (char*)malloc(sizeof(char) * (encodedFileSize)); // allocate enough memory in bytes.
		if (encodedFileBuffer == NULL) {
			exit(1);
			// todo print
		}
		//memset(encodedFileBuffer, 0, sizeof(char) * encodedFileSize);
		hamming(fileContentBuffer, encodedFileBuffer, fileSize);

		// Send the data through the socket
		int sent_bytes = send(txSocket, encodedFileBuffer, encodedFileSize, 0);
		printf(">: Sent: %d bytes\n", sent_bytes);
		
		free(fileContentBuffer); // Free used memory
		closesocket(txSocket);
	}

	return 0;
}
