#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#define MAX_BUF 1024

int create_pipe(char *name);

int write_pipe(int fd, char * buffer, int len);

int read_pipe(int fd, char *buf, int len);

int close_pipe(int fd, char *name);