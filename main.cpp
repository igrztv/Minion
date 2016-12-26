#include <stdlib.h>
#include <stdio.h>
#include "iostream"

#include <pthread.h>

#include "robotControl.h"
#include "gameObject.h"
#include "Spark.h"

#include "Client.h"
#include "Server.h"

#include "wrapper.h"

//////////////////////////////////////////////
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
//////////////////////////////////////////////

#define maybe(x) if((rand() % x) ? false : true)

#define delay(x) sleep(x)

using namespace std;

static void *ReadCommands(void *vptr_args);
static void *soundCommands(void *vptr_args);
static void onMouseMove(int, int, int, int, void*);
static bool issetCommand = false;
static bool voiceCommand = false;
static bool find_banana = false;
int SelfieCamera = 0;
int FrontCamera = 1;
static char command[100];
bool end_program = false;

int clientProcess(Client& client);
int serverProcess(Server& server);

char *address;

static bool send_file = false;
static bool send_photo = false;

Robot Minion(1500,1000,0);
Spark Accums(2);

bool USE_TCP = true;
bool WEB_ACCESS = true;

wrapper wrap;

int moveX = 0, moveY = 0;

static void onMouseMove(int event, int x, int y, int, void* a)
{
	if(event != EVENT_LBUTTONDOWN)
		return;

	//Robot*robot = (Robot*)Minion;
	//cout << "X, Y = " << x-50 << " " << y-50 << endl;
	moveY = ((x - 50) / 25.0);
	moveX = -((y - 50) / 25.0);
	cout << "moveX, Y = " << moveX << " " << moveY << endl;
}

void wrapperInit()
{
	wrap.add("frame", "<frame>");
	wrap.add("video", "<video>");
	wrap.add("selfie", "<selfie>");
	wrap.add("lower", "<lower>");
	wrap.add("swapCam", "<swapCam>");
	wrap.add("photo", "<photo>");
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
	wrap.add("music", "<music>");
}

int main(int argc, char* argv[])
{

	wrapperInit();
	cout << "wrapperInit()" << endl;
	
	//start program, read settings
	if(argc > 1)
	{
		for(int i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "NO_INTERNET") == 0)
			{
				cout << "WEB_ACCESS == false" << endl;
				WEB_ACCESS = false;
			}
			if(strcmp(argv[i], "NO_TCP") == 0)
			{
				cout << "USE_TCP == false" << endl;
				USE_TCP = false;
			}
			if(strcmp(argv[i], "-ip") == 0)
			{
				if(i + 1 < argc)
				{
					address = argv[i+1];
					i++;
					continue;
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

	pthread_t soundThread; 
	if (pthread_create(&soundThread, NULL, soundCommands, NULL) != 0)
	{
		cout << "\033[32mThread create returns with an error!\n\033[0m";
		return EXIT_FAILURE;
	}
	
	//set connecting params to reach controling server
	if(address == NULL)
		address = (char *)"127.0.0.1";
	cout << "connection address is " << address << endl;
	
	//Client client(address, 53000);
	Server server(53000);
	if(server.open() == 1){
		cout << "TCP server opened!"<< endl;
	}

	cout << "Open Minion\n";
	Minion.openPort("/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_0672FF495056805087183940-if02", 115200);
	if(Minion.isConnected())
	{
		delay(3);
		Minion.skipSettings();
		Minion.skipSettings();
		Minion.move_manual(Robot::MANUAL);
		cout << "\033[32mMinion connected!\033[0m\n";
	}else
		cout << "\033[31mERRROR open MINION. Port = " << Minion.port << "\033[0m\n";
	
	/*cout << "Open Lidar\n";
	Minion.lidar.openPort("/dev/serial/by-id/usb-Silicon_Labs_CP2103_USB_to_UART_Bridge_Controller_0001-if00-port0", 115200);
	if(!Minion.lidar.isConnected())
	{
		cout << "\033[31mERRROR open LIDAR. Port = " << Minion.lidar.port << "\033[0m\n";
	}else{
		cout << "\033[32mLidar connected!\033[0m\n";
	}*/
	
	cout << "Open Spark\n";
	Accums.openPort("/dev/serial/by-id/usb-Spark_Devices_Spark_Core_with_WiFi_6D8537A55151-if00", 115200);
	if(!Accums.isConnected())
	{
		cout << "\033[31mERRROR open Spark. Port = " << Accums.port << "\033[0m\n";
	}else{
		cout << "\033[32mSpark connected!\033[0m\n";
	}

	
	if(!USE_TCP)
	{
		cv::namedWindow("move", CV_WINDOW_NORMAL);
		cv::resizeWindow("move", 100, 100);
		Mat blank(100, 100, CV_8UC3, Scalar(0,0,0));
		imshow("move", blank);	
		setMouseCallback("move", onMouseMove);
	}
		int counter = 0;
	if(!USE_TCP)
		Minion.camera.CreateWindow();
	//videoCapture cam(0);

	while(!end_program)
	{
		
		if(USE_TCP)
			//clientProcess(client);
			serverProcess(server);
		else{
		if(send_photo)
		{
			voiceCommand = true;
			uint8_t* jpegBuff;
			int length = Minion.camera.GetFrame(jpegBuff, true, WEB_ACCESS);
			if(length < 0)
			{
				cout << "\033[32mSELFIE!\033[0m\n";
			}else
				cout << "\033[31mcannot parse jpeg\033[0m\n";
			send_photo = false;
		}
		}
		
		//Minion.lidar.Process();
		//Minion.lidar.segregate_areas();
		//Minion.correlation();
		
		Minion.sendCommands();
		Minion.getResponse();
	 
		Accums.getResponse();

		if(!USE_TCP)
		{
			//cout << "frame: \n";
			Minion.camera.ShowFrame();			
		}else{
			Minion.camera.NextFrame();
		}
		
		maybe(100)
		{
			Accums.getBatteries();
			if(Accums.plugged[0])
			{
				printf("Bat 1: %f%%,%f%%,%f%%,%f%%,%f%%;\n", Accums.cell[0][0],Accums.cell[0][1],Accums.cell[0][2],Accums.cell[0][3],Accums.cell[0][4]);
				printf("Bat 1: %u%%;\n", Accums.percent(0));
			}
		}
			
		if(issetCommand)
		{
			cout << "\033[33m" << command << "\033[0m" << endl;
			issetCommand = false;
		}	
		//Minion.Angle(Minion.Position().angle + d2r(10));
		if(!USE_TCP)
		{
			int key = cv::waitKey(25);
			switch(key)
			{
				case 'w': moveX = 1; break;
				case 'a': moveY = -1; break;
				case 's': moveX = -1; break;
				case 'd': moveY = 1; break;
				case '2': moveX = moveY = 0; break;
				case 27: end_program = true; break;
				case 'b': voiceCommand = true; break;
				case 'f': send_photo = true; break;
				case 'q': Accums.SelfieGetReady(); break;
				case 'e': Accums.SelfieRelease(); break;
				default: break;
			}
			//cout << "moveX, Y = " << moveX << " " << moveY << endl;
			Minion.MoveByPolar(moveX, moveY);
			counter++;
			if(counter > 90)
				counter = moveX = moveY = 0;
		}else{
			usleep(25000);
		}

	}
	
	if (pthread_join(readingThread, NULL) != 0)
		{
		cout << "cannot join readingThread!\n";
		return EXIT_FAILURE;
	}else{
		cout << "thread close success!\n";
	}
	if (pthread_join(soundThread, NULL) != 0)
		{
		cout << "cannot join soundThread!\n";
		return EXIT_FAILURE;
	}else{
		cout << "thread close success!\n";
	}

	return 0;
}

static void *soundCommands(void *vptr_args)
{
	char buf[100];
	printf("START sound cycles\n");
	while(!end_program)
	{
		printf("%c", 0);
		if(voiceCommand)
		{
			//cout << "\033[33m" << command << "\033[0m" << endl;
			sprintf(buf, "mplayer -ao alsa:device=hw=2.0 sounds/camera2.mp3");
			//"mplayer -ao alsa:device=hw=2.0 sounds/Game.mp3"
			printf("sound START\n");
			system(buf);
			printf("sound DONE\n");
			maybe(5)
			{
				sprintf(buf, "mplayer -ao alsa:device=hw=2.0 sounds/banana.mp3");
				printf("sound START\n");
				system(buf);
				printf("sound DONE\n");
			}
			voiceCommand = false;		
		}
	}
	printf("EXIT sound cycles!\n");
	return NULL;
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
			if(strcmp(str, "stop") == 0)
			{
				Minion.move(0, 0);
				continue; 
			}
			if(strcmp(str, "banana") == 0)
			{
				find_banana = !find_banana; 
				continue;
			}
			if(strcmp(str, "down") == 0)
			{
				Accums.SelfieGetReady();
				//Minion.selfie.OpenCamera(SelfieCamera);
			}
			if(strcmp(str, "up") == 0)
			{
				Accums.SelfieRelease();
				//Minion.selfie.OpenCamera(SelfieCamera);
			}
			if(strcmp(str, "photo") == 0)
			{
				send_photo = true;
			}
			if(strcmp(str, "swapCam") == 0)
			{
				int tmp = SelfieCamera;
				SelfieCamera = FrontCamera;
				FrontCamera = tmp;
				Minion.camera.CloseCamera();
				Minion.camera.OpenCamera(FrontCamera);
			}
			if(strcmp(str, "music") == 0)
				voiceCommand = true;

			memcpy(command, str, 100);
			issetCommand = true;
		}
	}
	return NULL;
}

int clientProcess(Client& client)
{
	//moveX = 0;
	//moveY = 0;
	static int reconnectionTimes = 0;
	if(client.connected)
	{
		//printf("connected\n");
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
				if(wrap.command[i].first == "photo") send_photo = true;
				if(wrap.command[i].first == "lower")
				{
					Accums.SelfieGetReady();
					//Minion.selfie.OpenCamera(SelfieCamera);
				}
				if(wrap.command[i].first == "swapCam")
				{
					int tmp = SelfieCamera;
					SelfieCamera = FrontCamera;
					FrontCamera = tmp;
					Minion.camera.CloseCamera();
					Minion.camera.OpenCamera(FrontCamera);
				}
				if(wrap.command[i].first == "music")
					voiceCommand = true;
			}
			if(moveX != 0 || moveY != 0)
				Accums.SelfieRelease();
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
		if(send_photo)
		{
			voiceCommand = true;
			uint8_t* jpegBuff;
			int length = Minion.camera.GetFrame(jpegBuff, true, WEB_ACCESS);
			if(length < 0)
			{
				cout << "\033[32mSELFIE!\033[0m\n";
			}else
				cout << "\033[31mcannot parse jpeg\033[0m\n";
			send_photo = false;
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

int serverProcess(Server& server)
{
	static int reconnectionTimes = 0;
	//if(client.connected)
	//{
		//printf("connected\n");
		char data[BYTES2READ + 1];
		//int read = client.readData(data);
		int read = server.readData(data);
        /*if (server.newClient){
            graphic->printText("I have new Client");
            graphic->change("stuart",1);
        }*/			
		if(read > 0)
		{
			cout << "\033[34m" << data << "\033[0m" << endl;
			wrap.unpack((uint8_t*)data, read);
			for(int i = 0; i < wrap.command.size(); ++i)
			{
				if(wrap.command[i].first == "video") send_file = true;
				if(wrap.command[i].first == "/video") send_file = false;
				if(wrap.command[i].first == "frame") cout << "received frame\n";
				if(wrap.command[i].first == "forward") moveX = 1;
				if(wrap.command[i].first == "backward") moveX = -1;
				if(wrap.command[i].first == "right") moveY = 1;
				if(wrap.command[i].first == "left") moveY = -1;
				if(wrap.command[i].first == "/forward") moveX = 0;
				if(wrap.command[i].first == "/backward") moveX = 0;
				if(wrap.command[i].first == "/right") moveY = 0;
				if(wrap.command[i].first == "/left") moveY = 0;
				if(wrap.command[i].first == "photo") send_photo = true;
				if(wrap.command[i].first == "lower") Accums.SelfieGetReady();
				if(wrap.command[i].first == "swapCam")
				{
					int tmp = SelfieCamera;
					SelfieCamera = FrontCamera;
					FrontCamera = tmp;
					Minion.camera.CloseCamera();
					Minion.camera.OpenCamera(FrontCamera);
				}
				if(wrap.command[i].first == "music")
					voiceCommand = true;
			}
			if(moveX != 0 || moveY != 0)
				Accums.SelfieRelease();
			Minion.MoveByPolar(moveX, moveY);
		}
		if(read < 0)
		{
			cout << "\033[31mlost connection\n\033[0m";
			Minion.move_manual(Robot::AUTO);
		}
		
	// }else{
	// 	maybe(100)
	// 	{
	// 		cout << "Trying reconnection...";
	// 		(client.open() < 0) ? cout << "\033[31mfail\033[0m\n" : cout << "\033[32mOK!\033[0m\n";
	// 		if(client.connected)
	// 		{				
	// 			Minion.move_manual(Robot::MANUAL);
	// 			reconnectionTimes = 0;
	// 		}else{
	// 			cout << "reconnectionTimes++;\n";
	// 			reconnectionTimes++;
	// 		}
	// 	}
	// }
		
	//if(client.connected)
	//{
		
		//send video frame
		if(send_file)
		{
			uint8_t* jpegBuff;
			int length = Minion.camera.GetFrame(jpegBuff);
			if(length > 0)
			{				
				wrap.pack("frame", jpegBuff, length);
				server.writeData(server.numsocks - 1, (char*)wrap.sendBuf, wrap.sendBufSize);
				//client.writeData((char*)wrap.sendBuf, wrap.sendBufSize);
			}else
				cout << "\033[31mcannot parse jpeg\033[0m\n";
		}
		if(send_photo)
		{
			voiceCommand = true;
			uint8_t* jpegBuff;
			int length = Minion.camera.GetFrame(jpegBuff, false, WEB_ACCESS);
			if(length > 0)
			{
				cout << "\033[32mSELFIE!\033[0m\n";
				//wrap.pack("selfie", jpegBuff, length);
				char len[3];
				sprintf(len, "%c%c", (length >> 8) & 0x00FF, length & 0x00FF);
				printf("\033[31mlength of picture: %d\033[0m\n", length);
				server.writeData(server.numsocks - 1, len, 2);
				server.writeData(server.numsocks - 1, (char*)jpegBuff, length);
			}else
				cout << "\033[31mcannot parse jpeg\033[0m\n";
			send_photo = false;
		}
		//send info about battery voltage
	/*}else{
		if(reconnectionTimes > 5)
			send_file = false;
		Minion.MoveByPolar(0, 0);
	}*/
	
	//accums
	maybe(100)
	{
		uint8_t percentage[1];
		if(Accums.plugged[0])
		{
			percentage[0] = Accums.percent(0);
			printf("Bat 1: %u%%;\n", percentage[0]);
			// if(client.connected)
			// {
			// 	wrap.pack("battery1", percentage, 1);		
			// 	client.writeData((char*)wrap.sendBuf, wrap.sendBufSize);
			// }
		}
		if(Accums.plugged[1])
		{
			percentage[0] = Accums.percent(1);
			printf("Bat 2: %u%%;\n",percentage[0]);
			// if(client.connected)
			// {
			// 	wrap.pack("battery2", percentage, 1);
			// 	client.writeData((char*)wrap.sendBuf, wrap.sendBufSize);
			// }
		}
	}
	
	return 0;
}
