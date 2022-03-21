#include "channelFunctions.h"

int main(int argc, char* argv[]) {
	// Check for cmdline args
	char* flag = "";
	int noiseAmt = 0;
	int randomNoiseSeed = 0;

	// User input
	char userInputBuffer[5];
	int amtBitsFlipped = 0;

	// recv buffer
	char recvBuf[MSG_SIZE];

	// Basic user greeting and helptext
	printf("-------[NOISY CHANNEL]------- \r\n\r\n");

	// Deal with argv
	if (argc <= 2) {
		printf("[INFO] A noise function flag was not provided.\r\n\tDefaulting to no noise in transmission!.\r\n\t");
		printf("To add random noise, use the -r <probability> <seed>, and for deterministic noise use -d <amount>.\r\n");
	}
	else {
		flag = argv[1];
		noiseAmt = atoi(argv[2]);
		if (argc == 4)
			randomNoiseSeed = atoi(argv[3]);
	}

	// Initialize sockets :)
	struct sockaddr_in sReceiver;
	struct sockaddr_in sSender;
	struct in_addr localIP = getThisPcAddr();
	int senderPort = 0;
	int recieverPort = 0;
	
	// Main action loop - create sockets for listening to the sender and reciever.
	SOCKET listenToSender =		createSocket(&localIP, 0, LISTEN, &sSender,		&senderPort);
	SOCKET listenToReceiver =	createSocket(&localIP, 0, LISTEN, &sReceiver,	&recieverPort);
	printf("[INFO] Started listening to the recieve socket, waiting for sender\r\n");
	printf("[INFO] This machine's IP is %s\r\n", inet_ntoa(localIP));
	printf("[INFO] \tSet sender target to IP:\t%s,\tport:%d\r\n", inet_ntoa(localIP), senderPort);
	printf("[INFO] \tSet reciever target to IP:\t%s,\tport:%d\r\n", inet_ntoa(localIP), recieverPort);
#ifdef DEBUG
	printf("[DEBUG] Noise flag %s, amt %d, seed %d\r\n", flag, noiseAmt, randomNoiseSeed);
#endif

	while (1) {
		// Allocate (too much) memory for message
		char* recvBuf = (char*)malloc(sizeof(char) * MSG_SIZE);
		if (recvBuf == NULL) {
			fprintf(stderr, "[ERR] Failed to allocate memory for recieving the message. Exiting");
			exit(1);
		}

		int senderParams = sizeof(struct sockaddr_in);
		int receiverParams = sizeof(struct sockaddr_in);
		SOCKET sockReciever = accept(listenToReceiver, (SOCKADDR*)&sReceiver, &receiverParams);
		SOCKET sockSender = accept(listenToSender, (SOCKADDR*)&sSender, &senderParams);
		printf("[INFO] Got two connections, one from Sender and one from Receiver!\r\n");

		int recievedMessageSize = recv(sockSender, recvBuf, MSG_SIZE, 0);

		recvBuf = (char*)realloc(recvBuf, recievedMessageSize + 1);
		recvBuf[recievedMessageSize + 1] = '\0';

		if (recievedMessageSize) {
			printf("\r\n>: Recieved %d bytes: \r\n", recievedMessageSize);
		}
		closesocket(sockSender);

		// Add noise to the buffered data here, depending on the flag provided by the user.
		if (!strcmp(flag, "-d")) {
			addDeterministicNoise(noiseAmt, recvBuf, recievedMessageSize, &amtBitsFlipped);
			printf("\r\n[INFO] Added deterministic noise!, flipped %d bits\r\n", amtBitsFlipped);
#ifdef DEBUG
			printf("\r\n>[INFO] After Noise: \r\n%s\r\n", recvBuf);
#endif
		}
		else if (!strcmp(flag, "-r")) {
			addRandomNoise(noiseAmt, randomNoiseSeed, recvBuf, recievedMessageSize, &amtBitsFlipped);
			printf("\r\n[INFO] Added random noise!, flipped %d bits\r\n", amtBitsFlipped);
		}

		int sentMessageSize = send(sockReciever, recvBuf, recievedMessageSize, 0);
		printf("[INFO] Sent %d bytes\r\n", sentMessageSize);
		closesocket(sockReciever);

		printf("\r\n>: Continue? (yes/no)\r\n");
		amtBitsFlipped = 0;
		scanf("%s", &userInputBuffer);
		if (!strcmp(userInputBuffer, "no")) {
			exit(0);
		}
	}

	return 0;
}
