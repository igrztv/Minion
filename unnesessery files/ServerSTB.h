#ifndef SERVER_H
#define SERVER_H

#include <iostream>

/* BufferLength is 100 bytes */
#define BufferLength 100
/* Server port number */
#define SERVPORT 53000
#define MAX_CLIENTS 10
#define BYTES2READ 5

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

//#include "Pipe.h"

using namespace std;

class Server
{
public:

	int sd, length;
	int clients[MAX_CLIENTS];
	int numsocks;
	int totalcnt, on;
	char buffer[BufferLength];
	struct sockaddr_in serveraddr;
	struct sockaddr_in their_addr;
	 
	fd_set read_fd, fd;
	struct timeval timeout;
	
	int port;
	
	int size;
	
	Server();
	Server(int port);
	void CloseClient(int client);
	~Server();
	
	int open();
	int readData(char *data);
	int readData();
	void writeData(int client, char* data);
};

#else

class Server
{
public:
	
	int open() { return -1; };
	Server(int port){ std::cout << "\nYOU CANNOT RUN TCP SERVER ON WINDOWS!\n\n"; }
	void readData(){}
	void writeData(int client, char* data){}
	~Server(){}
};

#endif

#endif
