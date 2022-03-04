#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library


void setBit(char* buffer, int bufferSize, int index, int value) {
	// Set specific bit in buffer
	buffer[index / bufferSize] &= ~(1u << (8 - (index % bufferSize)));
	// Set the bit
	if (value == 1) {
		buffer[index / bufferSize] |= (value << (8 - (index % bufferSize)));
	}
}

int getBit(char* buffer, int bufferSize, int index) {
	// Set specific bit from buffer
	int bit = (buffer[index / bufferSize] >> (7 - (index % bufferSize))) & 1;

	//int bit = buffer[index / bufferSize] &= ~(1u << (8 - (index % bufferSize)));
	return bit;

}

void hamming(char* originalFileBuffer, char* encodedFileBuffer, int originalFileLength) {

	for (int blockNumber = 0; blockNumber < originalFileLength; blockNumber += 26)
	{
		// Set data bits to correct places in the encoded file buffer

		// Do calculation for first byte
		
		// Do calculation for second byte

	}

}

int main(int argc, char* argv[]) {
	int stopUserInput = 0;
	char fileNameBuffer[3000];

	char* fileContentBuffer;

	// Test
	char buf = 0xFA;
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

		hamming(fileContentBuffer, encodedFileBuffer, fileSize);

		// Send the data through the socket
		int sent_bytes = send(txSocket, fileContentBuffer, fileSize, 0);
		printf(">: Sent: %d bytes\n", sent_bytes);
		
		free(fileContentBuffer); // Free used memory
		closesocket(txSocket);
	}

	return 0;
}
