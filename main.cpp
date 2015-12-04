#include <stdlib.h>
#include <stdio.h>
#include "iostream"

#include <pthread.h>

#include "robotControl.h"
#include "gameObject.h"
#include "Spark.h"
#include "Client.h"
//#include "Server.h"
#include "wrapper.h"

#define maybe(x) if((rand() % x) ? false : true)

#define delay(x) sleep(x)

using namespace std;

static void *ReadCommands(void *vptr_args);
static bool issetCommand = false;
static char command[100];
bool end_program = false;
int clientProcess(Client& client);
char *address;

Robot Minion(1500,1000,0);
Spark Accums(2);

bool USE_TCP = true;

wrapper wrap;

int main(int argc, char* argv[])
{
	
	wrap.add("frame", "<frame>");
	wrap.add("video", "<video>");
	wrap.add("/video", "</video>");
	wrap.add("forward", "<forward>");
	wrap.add("backward", "<backward>");
	wrap.add("left", "<left>");
	wrap.add("right", "<right>");
	wrap.add("/forward", "</forward>");
	wrap.add("/backward", "</backward>");
	wrap.add("/left", "</left>");
	wrap.add("/right", "</right>");
	wrap.add("speed", "<speed>");//speed koef
	wrap.add("battery1", "<battery1>");
	wrap.add("battery2", "<battery2>");
	
	//start program, read settings
	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "NO_TCP") == 0)
			{
				cout << argv[i] << " == false" << endl;
				USE_TCP = false;
			}
			if(strcmp(argv[i], "-ip") == 0)
			{
				if(i + 1 < argc)
				{
					address = argv[i+1];
					i++;
				}
			}
		}
	}
	
	//setup additional thread for manual controling via terminal
	pthread_t readingThread; 
    if (pthread_create(&readingThread, NULL, ReadCommands, NULL) != 0)
    {
		cout << "\033[32mThread create returns with an error!\n\033[0m";
        return EXIT_FAILURE;
    }
	
	//set connecting params to reach controling server
	if(address == NULL)
		address = (char *)"127.0.0.1";
	cout << "connection address is " << address << endl;
	Client client(address, 53000);

	cout << "Open Minion\n";
	Minion.openPort("/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_0672FF495056805087183940-if02", 115200);
	if(Minion.isConnected())
	{
		delay(3);
		Minion.skipSettings();
		Minion.skipSettings();
		cout << "\033[32mMinion connected!\033[0m\n";
	}else
		cout << "\033[31mERRROR open MINION. Port = " << Minion.port << "\033[0m\n";
	
	cout << "Open Lidar\n";
	Minion.lidar.openPort("/dev/serial/by-id/usb-Silicon_Labs_CP2103_USB_to_UART_Bridge_Controller_0001-if00-port0", 115200);
	if(!Minion.lidar.isConnected())
	{
		cout << "\033[31mERRROR open LIDAR. Port = " << Minion.lidar.port << "\033[0m\n";
	}else{
		cout << "\033[32mLidar connected!\033[0m\n";
	}
	
	cout << "Open Spark\n";
	Accums.openPort("/dev/serial/by-id/usb-Spark_Devices_Spark_Core_with_WiFi_6D8537A55151-if00", 115200);
	if(!Accums.isConnected())
	{
		cout << "\033[31mERRROR open Spark. Port = " << Accums.port << "\033[0m\n";
	}else{
		cout << "\033[32mSpark connected!\033[0m\n";
	}

	while(!end_program)
	{
		
		if(USE_TCP)
			clientProcess(client);
		
		Minion.lidar.Process();
		//Minion.lidar.segregate_areas();
		//Minion.correlation();
		
		Minion.sendCommands();
		Minion.getResponse();
		
		Accums.getResponse();
		
		maybe(100)
		{
			Accums.getBatteries();
		}
			
		if(issetCommand)
		{
			cout << "\033[33m" << command << "\033[0m" << endl;
			issetCommand = false;
		}	
		//Minion.Angle(Minion.Position().angle + d2r(10));
		usleep(25000);
	}
	
	if (pthread_join(readingThread, NULL) != 0)
    {
		cout << "cannot join readingThread!\n";
        return EXIT_FAILURE;
    }else{
		cout << "thread close success!\n";
	}

	return 0;
}

static void *ReadCommands(void *vptr_args)
{
	char str[100];
	while(true)
	{
		if(scanf("%s", str) > 0)
		{
			if(strcmp(str, "quit") == 0)
			{
				end_program = true;
				return NULL;
			}
			if(strcmp(str, "printData") == 0)
			{
				Minion.printData = !Minion.printData;
				Accums.printData = !Accums.printData;
				continue; 
			}
			if(strcmp(str, "manual") == 0)
			{
				Minion.move_manual(Robot::MANUAL);
				continue; 
			}
			if(strcmp(str, "auto") == 0)
			{
				Minion.move_manual(Robot::AUTO);
				continue; 
			}
			if(strcmp(str, "sparkWiFion") == 0)
			{
				Accums.WiFiOn();
				continue; 
			}
			if(strcmp(str, "sparkWiFioff") == 0)
			{
				Accums.WiFiOff();
				continue; 
			}
			if(strcmp(str, "SCO") == 0)
			{
				Accums.CloudOn();
				continue; 
			}
			if(strcmp(str, "left") == 0)
			{
				//Minion.speed.left = -120;
				//Minion.speed.right = 120;
				//Minion.move();
				continue; 
			}
			if(strcmp(str, "/left") == 0)
			{
				//Minion.speed.left = -1;
				//Minion.speed.right = 1;
				//Minion.move();
				continue; 
			}
			memcpy(command, str, 100);
			issetCommand = true;
		}
	}
	return NULL;
}

int clientProcess(Client& client)
{
	static bool send_file = false;
	static int moveX = 0;
	static int moveY = 0;
	static int reconnectionTimes = 0;
	if(client.connected)
	{
		
		char data[BYTES2READ];
		int read = client.readData(data);			
		if(read > 0)
		{
			cout << "\033[34m" << data << "\033[0m" << endl;
			wrap.unpack((uint8_t*)data, read);
			for(int i = 0; i < wrap.command.size(); ++i)
			{
				if(wrap.command[i].first == "video")
					send_file = true;
				if(wrap.command[i].first == "/video")
					send_file = false;
				if(wrap.command[i].first == "frame")
					cout << "received frame\n";
				if(wrap.command[i].first == "forward") moveX = 1;
				if(wrap.command[i].first == "backward") moveX = -1;
				if(wrap.command[i].first == "right") moveY = 1;
				if(wrap.command[i].first == "left") moveY = -1;
				if(wrap.command[i].first == "/forward") moveX = 0;
				if(wrap.command[i].first == "/backward") moveX = 0;
				if(wrap.command[i].first == "/right") moveY = 0;
				if(wrap.command[i].first == "/left") moveY = 0;
				/*if(wrap.command[i].first == "speed")
				{
					int koef = 0;
					if(sscanf((char*)wrap.command[i].second.second, "%d%%", &koef) == 1) //valid cmd: "<speed 3>98%"
						Minion.speed.koef = koef / 100.0;
					else
						cout << "\033[31mcannot parse SPEED koef\033[0m\n";
				}*/
			}
			Minion.MoveByPolar(moveX, moveY);
		}
		if(read < 0)
		{
			cout << "\033[31mlost connection\n\033[0m";
			Minion.move_manual(Robot::AUTO);
		}
		
	}else{
		maybe(100)
		{
			cout << "Trying reconnection...";
			(client.open() < 0) ? cout << "\033[31mfail\033[0m\n" : cout << "\033[32mOK!\033[0m\n";
			if(client.connected)
			{				
				Minion.move_manual(Robot::MANUAL);
				reconnectionTimes = 0;
			}else{
				cout << "reconnectionTimes++;\n";
				reconnectionTimes++;
			}
		}
	}
		
	if(client.connected)
	{
		
		//Minion.MoveByPolar(moveX, moveY);
		
		//send video frame
		if(send_file)
		{
			uint8_t* jpegBuff;
			int length = Minion.camera.GetFrame(jpegBuff);
			if(length > 0)
			{
				wrap.pack("frame", jpegBuff, length);
				client.writeData((char*)wrap.sendBuf, wrap.sendBufSize);
			}else
				cout << "\033[31mcannot parse jpeg\033[0m\n";
		}	
		//send info about battery voltage
	}else{
		if(reconnectionTimes > 5)
			send_file = false;
		Minion.MoveByPolar(0, 0);
	}
	
	maybe(100)
	{
		uint8_t percentage[1];
		if(Accums.plugged[0])
		{
			percentage[0] = Accums.percent(0);
			printf("Bat 1: %u%%;\n", percentage[0]);
			if(client.connected)
			{
				wrap.pack("battery1", percentage, 1);		
				client.writeData((char*)wrap.sendBuf, wrap.sendBufSize);
			}
		}
		if(Accums.plugged[1])
		{
			percentage[0] = Accums.percent(1);
			printf("Bat 2: %u%%;\n",percentage[0]);
			if(client.connected)
			{
				wrap.pack("battery2", percentage, 1);
				client.writeData((char*)wrap.sendBuf, wrap.sendBufSize);
			}
		}
	}
	
	return 0;
}
