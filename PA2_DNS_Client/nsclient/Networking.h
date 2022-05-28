#pragma once
#define BUFFER_SIZE 1024
#define UDP_PORT 53

WSADATA wsaData;
SOCKET sendSocket;
struct sockaddr_in netSocket;
#define TIMEOUT_MILLISECONDS 2000

void initUDP(char *serverIpAddr, unsigned int serverPort);
void closeUDPSocket();