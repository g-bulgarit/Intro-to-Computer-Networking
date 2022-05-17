#pragma once

#define BUFFER_SIZE  1024
#define TIMEOUT_SECONDS  2

void initUDP(char *serverIpAddr, unsigned int serverPort);
int sendUDP(char *buf, int bufferLength);
char* receiveUDP(int *recvBufferLength);
void closeUDPSocket();