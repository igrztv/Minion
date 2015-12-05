#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>

/* BufferLength is 100 bytes */
#define BufferLength 100
/* Server port number */
#define SERVPORT 53000
#define MAX_CLIENTS 10
#define BYTES2READ 40000
#define BUFF_SIZE 200

#ifndef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#endif

//#include "Pipe.h"

using namespace std;

class Server
{
public:

	unsigned char Buff[BUFF_SIZE];
	unsigned long Size;

	int sd, length;
	//vector<int> clients;
	int clients[MAX_CLIENTS];
	int numsocks;
	int totalcnt, on;
	char buffer[BufferLength];

#ifndef _WIN32
	struct sockaddr_in serveraddr;
	struct sockaddr_in their_addr;
	 
	fd_set read_fd, fd;
	struct timeval timeout;
#endif
	
	int port;	
	int size;

	void removeBegin(unsigned long cou);
	
	Server();
	Server(int port);
	void CloseClient(int client);
	~Server();
	
	int open();
	int readData(char *data);
	int readData();
	void writeData(int client, char* data, int len);

	int checkNewClient();

	friend ostream& operator<<(ostream& os, const Server& srvr);
};

#endif
