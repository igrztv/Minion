#include "SerialPort.h"

Serial::Serial()
{
	Size = 0;
	connected = false;
}

Serial::Serial(const char * portName, unsigned int baudRate)
{
	Size = 0;
	openPort(portName, baudRate);
}

Serial::~Serial()
{
	closePort();
	cout << "~Serial()\n";
}

int Serial::readData()
{
	int success = 1;//Windows flag
	DWORD bytesTransferred = BUFF_SIZE - Size;
	//cout << "file: " << __FILE__ << " line: " << __LINE__ << endl;

#ifdef _WIN32
	success = ReadFile(port, Buff+Size, bytesTransferred, &bytesTransferred, NULL);
#else
	bytesTransferred = read(port, Buff+Size, bytesTransferred);
#endif

	//cout << "file: " << __FILE__ << " line: " << __LINE__ << endl;

	if (!success)
	{
		//fprintf(stderr, "Error: Unable to read Get Position response from serial port.  Error code 0x%x.", GetLastError());
		return 0;
	}else{
		if(bytesTransferred > 0)
		{
			Size += bytesTransferred;
			if(printData == true)
			{
				for(int i = Size - bytesTransferred; i < Size; i++)
				{
					//printf("\t{%d %x %c}\n", response[i], response[i], response[i]);
					printf("%x ", Buff[i]);
				}
				printf("\n");
				for(int i = Size - bytesTransferred; i < Size; i++)
				{
					printf("%c", Buff[i]);
				}
				cout << endl;
			}
		}else{
			return 0;
		}
	}

	return bytesTransferred;
}

int Serial::writeData(unsigned char *buffer, int length)
{
	int success = 1;//Windows flag
	DWORD bytesTransferred = 0;

#ifdef _WIN32
	success = WriteFile(port, buffer, length, &bytesTransferred, NULL);
#else
	usleep(5000);
	bytesTransferred = write(port, buffer, length);
	if(printData)
	{
		for(int i = 0; i < bytesTransferred; ++i)
		{
			printf("%x ", buffer[i]);
		}
		cout << endl;
		for(int i = 0; i < bytesTransferred; ++i)
		{
			printf("%c", buffer[i]);
		}
		cout << endl;
	}
#endif

	if (!success)
	{
		//fprintf(stderr, "Error: Unable to write Get Position command to serial port.  Error code 0x%x.", GetLastError());
		return 0;
	}
	if (length != bytesTransferred)
	{
		fprintf(stderr, "Error: Expected to write %d bytes but only wrote %lu.", length, bytesTransferred);
		return 0;
	}

	return bytesTransferred;
}

void Serial::removeBegin(DWORD cou)
{
	if(Size-cou < 0) return;
	memcpy(Buff,Buff+cou,Size-cou);
	Size -= cou;
}

#ifdef _WIN32

HANDLE Serial::openPort(const char * portName, unsigned int baudRate)
{
	DCB commState;
	int success;
	COMMTIMEOUTS timeouts;

	/* Open the serial port. */
	port = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (port == INVALID_HANDLE_VALUE)
	{
		switch(GetLastError())
		{
		case ERROR_ACCESS_DENIED:	
			fprintf(stderr, "Error: Access denied.  Try closing all other programs that are using the device.\n");
			break;
		case ERROR_FILE_NOT_FOUND:
			fprintf(stderr, "Error: Serial port not found. Make sure that \"%s\" is the right port name.\n", portName);
			break;
		default:
			fprintf(stderr, "Error: Unable to open serial port.  Error code 0x%x.\n", GetLastError());
			break;
		}
		return INVALID_HANDLE_VALUE;
	}

	/* Set the timeouts. */
	success = GetCommTimeouts(port, &timeouts);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to get comm timeouts.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	success = SetCommTimeouts(port, &timeouts);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to set comm timeouts.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	/* Set the baud rate. */
	success = GetCommState(port, &commState);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to get comm state.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}
	commState.BaudRate = baudRate;
	success = SetCommState(port, &commState);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to set comm state.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	/* Flush out any bytes received from the device earlier. */
	success = FlushFileBuffers(port);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to flush port buffers.  Error code 0x%x.\n", GetLastError());
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	if(port != INVALID_HANDLE_VALUE)
		connected = true;

	return port;
}

void Serial::closePort()
{
	if(port)
	{
		CloseHandle(port);
		port = NULL;
	}
}

#else

void Serial::closePort()
{
	if(port)
	{
		close(port);
		port = 0;
	}
}

HANDLE Serial::openPort(const char *portName, unsigned int baudrate)
{

	port = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
	if (port == -1){
		//Could not open the port.
		perror("open_port: Unable to open /dev/ttyUSBx - ");
	}else
		fcntl(port, F_SETFL, 0);

	int parity = 0;

	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (port, &tty) != 0){
		perror("error %d from tcgetattr");
	}

	if(baudrate == (unsigned int) 115200)
	{
		cfsetospeed (&tty, B115200);
		cfsetispeed (&tty, B115200);
	}

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;	 // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;		 // ignore break signal
	tty.c_lflag = 0;				// no signaling chars, no echo,
									// no canonical processing
	tty.c_oflag = 0;				// no remapping, no delays
	tty.c_cc[VMIN]  = 0;			// read doesn't block
	tty.c_cc[VTIME] = 0;			// 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
									// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);	  // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (port, TCSANOW, &tty) != 0){
		perror("error %d from tcsetattr");
	}

	int should_block = 0;	// set no blocking
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (port, &tty) != 0){
		perror("error %d from tggetattr");
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 0;			// 0.5 seconds read timeout

	if (tcsetattr (port, TCSANOW, &tty) != 0)
			perror("error %d setting term attributes");
			
	if(port != INVALID_HANDLE_VALUE)
		connected = true;
	return port;
}

#endif
