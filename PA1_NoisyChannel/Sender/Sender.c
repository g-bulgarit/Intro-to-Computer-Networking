#include "helperFunctions.h"

// Code for the sender application, that takes in a file specified by the user (limitation - the file size must be a multiple of 26 bits)
// and transfers it over a noisy channel to a receiver application.
// To overcome the noise - the sender encodes the data using hamming(26, 31, 3) and the receiver decodes it.

int main(int argc, char* argv[]) {

	// Define variables 
	int stopUserInput = 0;
	char fileNameBuffer[MAX_FILENAME_LENGTH];
	char* fileContentBuffer;
	FILE* rfp;

#ifdef DEBUG
	printf("-------[SENDER]------- \r\n\r\n");
#endif

	// Check for cmdline args
	if (argc != 3) {
		printf("[ERR] Please provide all required command-line arguments.\r\n. Sender.exe <ip_address> <port>\r\n");
		exit(1);
	}

	// Set up connection 
	int port = atoi(argv[2]);
	SOCKET txSocket;
	struct sockaddr_in remote;
	// Create the TX socket
	
	while (!stopUserInput) {
		// Connect to server (channel)
		txSocket = createSocket(argv[1], port, SEND, &remote);
		int listen_retcode = connect(txSocket, (SOCKADDR*)&remote, sizeof(remote));

#ifdef DEBUG
		printf("[DEBUG] Starting sender with IP %s and port %d\r\n", argv[1], port);
#endif

		printf("[INFO] Connected to the Noisy Channel\r\n");
		// Get filename from user
		printf(">: Enter file name (or type quit (...to quit)): ");
		scanf("%s", &fileNameBuffer);

		if (!strcmp(fileNameBuffer, "quit"))
			exit(0);

		// Check if the file exists and open it.
		rfp = fopen(fileNameBuffer, "rb");
		if (rfp == NULL) {
			fprintf(stderr, "[ERR] Failed to open file for reading. Exiting.");
		}

		// Get file size: taken partly from https://codereview.stackexchange.com/questions/112613/memory-safe-file-reading-in-c
		fseek(rfp, 0, SEEK_END); // get offset at end of file
		int fileSize = ftell(rfp); // Get size
		rewind(rfp, 0, SEEK_SET); // seek back to beginning
		fileContentBuffer = (char*)malloc(sizeof(char) * (fileSize) + 1); // allocate enough memory in bytes +1 for null

		// Read the file to our buffer
		if (rfp != NULL) {
			size_t newLen = fread(fileContentBuffer, sizeof(char), fileSize, rfp);
			if (ferror(rfp) != 0) {
				fprintf(stderr, "[ERR] Error reading input file, make sure the path is correct. Exiting.");
				exit(1);
			}
			else {
				fileContentBuffer[newLen++] = '\0';
			}
			fclose(rfp);
		}
		else {
			fprintf(stderr, "[ERR] Failed to allocate enough memory for reading the file.\r\nExiting.");
			exit(1);
		}

		// Encode with hamming(31,26,3):
		//		- allocate space for the encoded buffer using the message lengths as a convertion unit, fill with zeros
		//		- Go over the bits and fill in control bits

		int encodedFileSize = (int)(fileSize) * (31.0 / 26.0);
		char* encodedFileBuffer = (char*)malloc(sizeof(char) * (encodedFileSize)); // allocate enough memory in bytes.
		if (encodedFileBuffer == NULL) {
			fprintf(stderr, "[ERR] Failed to allocate enough memory for the encoded file buffer.\r\nExiting.");
			exit(1);
		}
		memset(encodedFileBuffer, 0, sizeof(char) * encodedFileSize);

		// Encode with hamming code
		hamming(fileContentBuffer, encodedFileBuffer, fileSize);

		// Send the data through the socket
		int sent_bytes = send(txSocket, encodedFileBuffer, encodedFileSize, 0);
		printf("[INFO] Sent: %d bytes\n", sent_bytes);

		free(fileContentBuffer); // Free used memory for the original file
		free(encodedFileBuffer); // Free used memory for the encoded file

		// Close the TX socket
		closesocket(txSocket);
	}

	
	return 0;
}
