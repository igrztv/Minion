#include "Server.h"

void Server::removeBegin(unsigned long cou)
{
	if(Size-cou < 0) return;
	memcpy(Buff,Buff+cou,Size-cou);
	Size -= cou;
}

ostream& operator<<(ostream& os, const Server& srvr)
{
	os << srvr.Buff << endl;
	return os;
}

#ifndef _WIN32

Server::Server()
{
	size = 0;
}

Server::Server(int _port)
{
	port = _port;
}

int Server::open()
{
	
	length = sizeof(int);
	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;	
	size = 0;

	/* Get a socket descriptor */
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Server-socket() error");
		return -1;
	}else
		printf("Server-socket() is OK\n");

	// where socketfd is the socket you want to make non-blocking
	int status = fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) | O_NONBLOCK);

	if (status == -1){
		perror("calling fcntl");
	}
	 
	/* Allow socket descriptor to be reusable */
	int rc = 0;
	if((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(/*on*/int))) < 0)
	{
		perror("Server-setsockopt() error");
		close(sd);
		return -1;
	}else
		printf("Server-setsockopt() is OK\n");
	 
	/* bind to an address */
	memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	 
	//printf("Using %s, listening at %d\n", inet_ntoa(serveraddr.sin_addr), SERVPORT);
    bool isUsed = true;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(char*)(&isUsed), sizeof(isUsed));
	if((rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0)
	{
		perror("Server-bind() error");
		/* Close the socket descriptor */
		close(sd);
		/* and just exit */
		return -1;
	}else
		printf("Server-bind() is OK\n");
	 
	/* Up to MAX_CLIENTS clients can be queued */
	if((rc = listen(sd, MAX_CLIENTS)) < 0)
	{
		perror("Server-listen() error");
		close(sd);
		return -1;
	}
	
	printf("Server-Ready for client connection...\n");	
	numsocks = 0;
	 
	/* accept() the incoming connection request. */
	//int sin_size = sizeof(struct sockaddr_in);
	//if((clients[0] = accept(sd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size)) < 0)
	//{
	//	perror("Server-accept() error");
	//	close(sd);
	//	return -1;
	//}
	
	printf("Server-accept() is OK\n");
	FD_ZERO(&fd);
	FD_SET(sd, &fd);
		
	/*client IP*/
	printf("Server-new socket, clients[0] is OK...\n");
	return 1;
	//printf("Got connection from the f***ing client: %s\n", inet_ntoa(their_addr.sin_addr));
}

//http://stackoverflow.com/questions/4200172/c-socket-programming-connecting-multiple-clients-to-server-using-select

int Server::readData()
{
	char data[BYTES2READ];
	return readData(data);
}

int Server::checkNewClient()
{
	if(FD_ISSET(sd, &read_fd))
	{
		if (numsocks < MAX_CLIENTS)
		{
			/* accept() the incoming connection request. */
			int sin_size = sizeof(struct sockaddr_in);
			int handle = 0;
			if((handle = accept(sd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size)) < 0)
			{
				perror("Server-accept() error");
				close(sd);
				return -1;
			}
			clients[numsocks] = handle;
			FD_SET(clients[numsocks], &fd);
									
			cout << "NEW clients[" << numsocks << "] => " << handle << endl;
			numsocks++;
		} else {
			printf("Ran out of socket space.\n");
			close(sd);
			return -1;
		}
	}
	return 0;
}

int Server::readData(char* data)
{
	read_fd = fd;
	long int rc = select(FD_SETSIZE, &read_fd, NULL, NULL, &timeout);
	if (rc == -1) {
		perror("Server-select() error");
		close(sd);
		return -1;
	}
	
	if(checkNewClient() < 0)
		return -1;
	
	for(int client = 0; client < numsocks; ++client)
	{
		if(FD_ISSET(clients[client], &read_fd))
		{
			char recc[BYTES2READ + 1];
			rc = read(clients[client], recc, BYTES2READ);
			if(rc < 1)
			{
				if(rc)
					perror("Server-read() error");
				else
					perror("Client program has issued a close()");
				CloseClient(client);
				return 0;
			}else{				
				recc[rc + 1] = '\0';
				memcpy(data, recc, BYTES2READ + 1);
				memset(recc, 1, BYTES2READ + 1);
				return rc;
			}
		}
	}
	
	return 0;
}

Server::~Server()
{
	for(int client = 0; client < numsocks; ++client)
		close(clients[client]);
	close(sd);
}

void Server::CloseClient(int client)
{
	cout << "CloseClient: " << clients[client] << endl;
	close(clients[client]);
	FD_CLR(clients[client], &fd);
}

void Server::writeData(int client, char* data, int len)
{
	if(!FD_ISSET(clients[client], &fd))
		return;
	//cout << "writeData clients[" << client << "]\n";
    int dataLen = (len > 0) ? len : strlen(data);
	int rc = write(clients[client], data, dataLen);
	if(rc != dataLen)
	{
		cout << "write returns: " << rc << endl;
		perror("Server-write() error");
		char temp = 0;
		rc = getsockopt(clients[client], SOL_SOCKET, SO_ERROR, &temp, (socklen_t *)&length);
		if(rc < 1)
		{
			errno = temp;
			perror("SO_ERROR was: ");
		}					 
		CloseClient(client);
	}
}

#else

	int Server::open() { return -1; };
	Server::Server(int port){ std::cout << "\nYOU CANNOT RUN TCP SERVER ON WINDOWS!\n\n"; }
	int Server::readData(){return 0;}
	int Server::readData(char *data){return 0;}
	void Server::writeData(int client, char* data){}
	Server::~Server(){}

	void Server::CloseClient(int client) {}

	void Server::checkNewClient() {}

#endif
	
