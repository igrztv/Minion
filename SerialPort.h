#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <stdio.h>      // standard input / output functions

#ifdef _WIN32

	#include <windows.h>
	int test(char * portName);

#else

	#include <stdlib.h>
	#include <string.h>     // string function definitions
	#include <unistd.h>     // UNIX standard function definitions
	#include <fcntl.h>      // File control definitions
	#include <errno.h>      // Error number definitions
	#include <termios.h>    // POSIX terminal control definitions
	#include <iostream>

	typedef int HANDLE;
	typedef unsigned char byte;
	typedef unsigned short WORD;
	typedef unsigned long DWORD;
	#define INVALID_HANDLE_VALUE -1	

#endif

using namespace std;

#define BUFF_SIZE 2000

class Serial
{
private:
	bool connected;
public:
	unsigned char Buff[BUFF_SIZE];
	DWORD Size;

	HANDLE port;

	Serial(const char * portName, unsigned int baudRate);
	Serial();
	~Serial();
	HANDLE openPort(const char * portName, unsigned int baudRate);
	bool isConnected(){ return connected; };
	void closePort();
	int readData();
	void removeBegin(DWORD cou);
	int writeData(unsigned char *buffer, int length);
	bool printData;
};

#endif
