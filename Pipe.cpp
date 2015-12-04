#include "Pipe.h"

int create_pipe(char *name)
{	
	int fd;   // file descriptor

	// create pipes to monitor (if they don't already exist)
	//system("mkfifo /tmp/PIPE");
	
	if (mkfifo(name, 0666) < 0) { 
		//printf("\n %s \n", strerror(errno));
		cout << "error creating file\n";
		return 0;
	}

	// open file descriptors of named pipes to watch
	fd = open(name, O_RDWR | O_NONBLOCK);
	if (fd == -1) {
		perror("open error");
		return EXIT_FAILURE;
	}else{
		return fd;
	}
}

int write_pipe(int fd, char * buffer, int len)
{
	cout << buffer << endl;
    return write(fd, buffer, len);
}

int read_pipe(int fd, char *buf, int len)
{
		fd_set read_fds;        // file descriptor read flags
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 50000;	
		
		FD_ZERO(&read_fds);
		FD_SET(fd, &read_fds);
		int nfd = select(fd+1, &read_fds, NULL, NULL, &tv);
		if (nfd != 0) {
			if (nfd == -1) {
				cout << "select(): error\n";
				return EXIT_FAILURE;
			}
			if (FD_ISSET(fd, &read_fds)) {
				ssize_t bytes;
				size_t total_bytes = 0;

				bytes = read(fd, buf, len);
				if (bytes > 0) {
					total_bytes += (size_t)bytes;
					//cout << "Read: " << buffer << endl;
					return bytes;
				} else {
					if (errno == EWOULDBLOCK) {
						cout << "read(): EWOULDBLOCK\n";
						return EXIT_FAILURE;
					} else {
						cout << "read(): error\n";
						return EXIT_FAILURE;
					}
				}
			}else{
				cout << "select(): no data\n";
				return 0;
			}
		}else{
			//cout << "select(): time out\n";
			return 0;
		}
		
        return EXIT_SUCCESS;
}

int close_pipe(int fd, char *name)
{
	close(fd);
    unlink(name);
    return 0;
}