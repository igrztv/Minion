#include "Server.h"

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
	totalcnt = 0;
	on = 1;
	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;	
	size = 0;

	/* Get a socket descriptor */
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Server-socket() error");
		/* Just exit */
		return -1;
	}else
		printf("Server-socket() is OK\n");

	// where socketfd is the socket you want to make non-blocking
	int status = fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) | O_NONBLOCK);

	if (status == -1){
		perror("calling fcntl");
		// handle the error.  By the way, I've never seen fcntl fail in this way
	}
	 
	/* Allow socket descriptor to be reusable */
	int rc = 0;
	if((rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
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

int Server::readData(char* data)
{
	read_fd = fd;
	int rc = select(FD_SETSIZE, &read_fd, NULL, NULL, &timeout);
	if (rc == -1) {
		perror("Server-select() error");
		close(sd);
		exit(-1);
	}
	
	if(FD_ISSET(sd, &read_fd))
	{
		if (numsocks < MAX_CLIENTS)
		{
			/* accept() the incoming connection request. */
			int sin_size = sizeof(struct sockaddr_in);
			if((clients[numsocks] = accept(sd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size)) < 0)
			{
				perror("Server-accept() error");
				close(sd);
				exit(-1);
			}
			
			FD_SET(clients[numsocks], &fd);							
			cout << "new Client accepted!\n";
			cout << "clients[numsocks] " << clients[numsocks] << endl;
			numsocks++;
		} else {
			printf("Ran out of socket space.\n");
			close(sd);
			exit(-1);
		}
	}
	
	for(int client = 0; client < numsocks; ++client)
	{
		if(FD_ISSET(clients[client], &read_fd))
		{				
			/* read() from client */
			char recc[BYTES2READ];
			rc = read(clients[client], recc, BYTES2READ-1);
			if(rc < 1)
			{
				if(rc)
					perror("Server-read() error");
				else
					printf("Client program has issued a close()\n");
				CloseClient(client);
				return 0;
			}else{
				//printf("%x\n", recc[0]);
				//if(recc[0] == '\n')
				//{
					cout << "\033[32m";
					//cout << size << " ";
					cout << recc;
					for(int i = 0; i < rc; i++)
					{
						cout << buffer[i];
					}
					buffer[rc] = '\0';
					cout << "\033[0m" << endl;
					//size = 0;
					//memset(buffer, 0, BufferLength);
				/*}else{
					buffer[size] = recc[0];
					size++;
					if(size >= BufferLength)
					{
						memset(buffer, 0, BufferLength);
						size = 0;
						cout << "size >= BufferLength\n" << endl;
					}
				}*/	

				//cout << "writeData(" << clients[client] << ", " << recc << ")\n";
				writeData(client, recc);
				memcpy(data, recc, BYTES2READ);
				memset(recc, 1, BYTES2READ);
				return BYTES2READ;
			}
		}
	}
	
	return 0;
}

Server::~Server()
{
	//close(clients[0]);
	
	close(sd);
	exit(0);
}

void Server::CloseClient(int client)
{
	cout << "CloseClient: " << clients[client] << endl;
	close(clients[client]);
	FD_CLR(clients[client], &fd);
}

void Server::writeData(int client, char* data)
{
	int dataLen = sizeof(data);
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



#endif

	
	/*int pipe = create_pipe("/tmp/PIPE_R");
	int pipe2 = create_pipe("/tmp/PIPE_W");
	cout << "pipe id: " << pipe << endl;
	cout << "pipe2 id: " << pipe2 << endl;	
	//char pipe_buf[10] = "HarryMorgan";
	char pipe2_buf[10] = "IgorZotov";
	char pipe2_read[10];
	char pipe_read[10];
	int i = 0;
	while(i < 10)
	{
		cout << "Write PIPE2: " << write_pipe(pipe2, pipe2_buf+i, 1) << endl;
		if(read_pipe(pipe2, pipe2_read+i, 1) > 0)
		{
			cout << "READ PIPE2: " << pipe2_read[i] << endl;
		}else{
			cout << "READ PIPE2: empty" << endl;
		}
		cout << "Write PIPE: " << write_pipe(pipe, pipe2_read+i, 1) << endl;
		if(read_pipe(pipe, pipe_read+i, 1) > 0)
		{
			cout << "READ PIPE: " << pipe_read[i] << endl;
		}else{
			cout << "READ PIPE: empty" << endl;
		}
		i++;
	}

	close_pipe(pipe, "/tmp/PIPE_R");
	close_pipe(pipe2, "/tmp/PIPE_W");*/
	
