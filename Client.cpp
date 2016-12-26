#include "Client.h"

Client::Client()
{
}

Client::Client(const char* ip, int _port) : address((char*)ip), port(_port), connected(false), sockfd(0)
{
}

int Client::open()
{
	
	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;	

    struct sockaddr_in serv_addr;
    struct hostent *server;
    int status = 0;

    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if(sockfd < 0)
	{
        perror("Client error socket()");
		return -1;
	}

    server = gethostbyname(address);  
    if (server == NULL) {
        fprintf(stderr,"Client error, no such host\n");
        return -1;
    }
	
	status = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
    if (status == -1)
    {
		perror("Client error calling fcntl");
		return -1;
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    
    status = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
    if (status == -1)
	{
		//perror("Client error connecting");
		//return -1;
	}
	
	FD_ZERO(&fd);
	FD_SET(sockfd, &fd);
	
	if(readData() < 0)
	{
		return -1;
	}

	connected = true;
	return 1;
}

//http://stackoverflow.com/questions/4200172/c-socket-programming-connecting-multiple-clients-to-server-using-select

int Client::readData()
{
	char data[BYTES2READ];
	return readData(data);
}

int Client::readData(char* data)
{
	
	read_fd = fd;
	int rc = select(FD_SETSIZE, &read_fd, NULL, NULL, &timeout);
	if (rc < 0) {
		perror("Client-select() error");
		CloseClient();
		return -1;
	}

	//check whether any new connection state available
	if(FD_ISSET(sockfd, &read_fd))
	{
		//fill incomming buffer '1' as default value
		char recc[BYTES2READ] = {1};
		rc = read(sockfd, recc, BYTES2READ-1);
		if(rc == 0)
		{
			perror("No connection: ");
			CloseClient();
			return -1;
		}
		if(rc < 0)
		{
			//perror("\033[31mConnection lost\n\033[0m");
			CloseClient();
			return -1;
		}
		if(rc > 0)
		{			
			recc[rc] = '\0';
			memcpy(data, recc, BYTES2READ);
			return rc;
		}
	}
	
	return 0;
}

Client::~Client()
{
	CloseClient();
	cout << "~Client()\n";
}

void Client::CloseClient()
{
	//cout << "CloseClient: " << sockfd << endl;
	close(sockfd);
	FD_CLR(sockfd, &fd);
	connected = false;
}

int Client::writeData(char* data, long int length)
{
	long int dataLen = (length > 0) ? length : strlen(data);
	if (dataLen < 0)
	{
		cout << "dataLen < 0\n";
		return -1;
	}
	int rc = write(sockfd, data, dataLen);
	if(rc < 0)
	{
		//cout << "write status == " << rc << "; "; 
		perror("\033[31mClient-write() error\033[0m");
		char temp = 0;
		int sockLength = 0;
		rc = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &temp, (socklen_t *)&sockLength);
		if(rc < 0)
		{
			//cout << "getsockopt() returns: " << rc << "; ";
			perror("error: ");
			CloseClient();
			return -1;
		}
		CloseClient();
		return -1;		
	}
	return rc;
}
