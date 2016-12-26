#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>

/* Server port number */
#define SERVPORT 53000
#define BYTES2READ 40000

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
#include <netdb.h> 

using namespace std;

class Client
{
public:

	int sockfd;
	int numsocks;
	bool connected;

	struct sockaddr_in serveraddr;
	struct sockaddr_in their_addr;
	 
	fd_set read_fd, fd;
	struct timeval timeout;
	
	int port;
	char *address;
	
	Client();
	Client(const char* ip, int port);
	void CloseClient();
	~Client();
	
	int open();
	int readData(char *data);
	int readData();
	int writeData(char* data, long int length = -1);

	void checkNewClient();

};

#endif
