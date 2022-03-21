#include "receiverFunctions.h"


int main(int argc, char* argv[]) {
	FILE* wfp;
	char fileNameBuffer[3000];
	int fixedBits = 0;

	printf("-------[RECIEVER]------- \r\n\r\n");
	// Check for cmdline args
	if (argc != 3) {
		fprintf(stderr, "[ERR] Please provide all required command-line arguments.\r\n Reciever.exe <ip_address> <port>\r\n");
		exit(1);
	}

	// Set up connection details
	int port = atoi(argv[2]);
	struct sockaddr_in server;

	SOCKET rxSocket;
	printf("[INFO] Started socket, connecting to the noisy channel...\r\n");

	while (1) {
		// Setup recv buffer
		char* recvBuf = (char*)malloc(sizeof(char) * MSG_SIZE + 1);
		if (recvBuf == NULL) {
			fprintf(stderr, "[ERR] Failed to allocate space for the recieve-buffer. Exiting.");
			exit(1);
		}

		rxSocket = createSocket(argv[1], port, SEND, &server);
		int c = sizeof(struct sockaddr_in);
		int recievedMessageSize = 0;
		int connectRetcode = connect(rxSocket, (SOCKADDR*)&server, sizeof(server));
		printf("[INFO] Connected successfully, waiting to recieve a message.\r\n");
		recievedMessageSize = recv(rxSocket, recvBuf, MSG_SIZE, 0);

		recvBuf = (char*)realloc(recvBuf, recievedMessageSize);
		//recvBuf[recievedMessageSize] = '\0';

		if (recievedMessageSize) {
			printf("[INFO] Recieved %d bytes\r\n", recievedMessageSize);
#ifdef DEBUG
			printf("[INFO] Recieved:\r\n%s\r\n", recvBuf);
#endif
		}

		// Decode the file here
		int decodedFileSize = recievedMessageSize * (26.0 / 31.0);
		char* decodedFileBuffer = (char*)malloc(sizeof(char) * decodedFileSize);
		if (decodedFileBuffer == NULL) {
			fprintf(stderr, "[ERR] Failed to allocate space for the decoded message buffer. Exiting");
			exit(1);
		}

		// Set entire buffer to 0.
		memset(decodedFileBuffer, 0, sizeof(char) * decodedFileSize);

		unhamming(recvBuf, decodedFileBuffer, recievedMessageSize, &fixedBits);
		//decodedFileBuffer[decodedFileSize + 1] = '\0';
		printf("[Decode] After decoding, left with %d bytes\r\n", decodedFileSize);
		printf("[Decode] Overall, fixed %d flipped bits\r\n", fixedBits);
#ifdef DEBUG
		printf("[Decode] Decoded:\r\n%s\r\n", decodedFileBuffer);
#endif


		// Save the file with the help of some user input
		printf("\r\n>: Enter filename (or type quit to quit): ");
		scanf("%s", &fileNameBuffer);

		// Check if the user wants to quit
		if (!strcmp(fileNameBuffer, "quit"))
			exit(0);

		wfp = fopen(fileNameBuffer, "wb+");
		fwrite(decodedFileBuffer, 1, decodedFileSize, wfp);
		fclose(wfp);
		fixedBits = 0;

		// Free memory
		free(recvBuf);
		free(decodedFileBuffer);

		// Close the socket before proceeding
		closesocket(rxSocket);
	}
	return 0;
}
