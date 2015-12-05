#include "Spark.h"

Spark::Spark(int NumberOfAccums) : acb(NumberOfAccums)
{
	voltage[0] = voltage[1] = 0;
	plugged[0] = plugged[1] = false;
	StickIsDown = false;
}

void Spark::WiFiOn()
{
	unsigned char command[1];
	command[0] = WIFIon;
	writeData(command, 1);
}

void Spark::WiFiOff()
{
	cout << "Sperk WiFi OFF\n";
	unsigned char command[1];
	command[0] = WIFIoff;
	writeData(command, 1);
}

void Spark::CloudOn()
{
	unsigned char command[1];
	cout << "Sperk Cloud ON\n";
	command[0] = CLOUDon;
	writeData(command, 1);
}

void Spark::CloudOff()
{
	unsigned char command[1];
	command[0] = CLOUDoff;
	writeData(command, 1);
}

void Spark::WiFiParams(const char* SSID, const char* password)
{
	int length = 1 + strlen(SSID) + 1 + strlen(password) + 1;
	unsigned char* command = new unsigned char[length];
	command[0] = WIFIparam;
	memcpy(command + 1, SSID, strlen(SSID));
	command[1 + strlen(SSID)] = StopByte;
	memcpy(command + 1 + 1 + strlen(SSID), password, strlen(password));
	command[1 + 1 + strlen(SSID) + strlen(password)] = StopByte;
	
	writeData(command, length);	
}

void Spark::SelfieGetReady()
{
	if(StickIsDown)
		return;
	unsigned char command[1];
	command[0] = SlefieDown;
	writeData(command, 1);
	StickIsDown = true;
}

void Spark::SelfieRelease()
{
	if(StickIsDown)
	{
		unsigned char command[1];
		command[0] = SlefieUp;
		writeData(command, 1);
		StickIsDown = false;
	}
}

bool Spark::getResponse()
{
	if(!isConnected()) return false;

	int count = readData();

	while(Size > 0)
	{					
		if(Buff[0] == ACB)
		{
			bool frameIsFull = false;
			int i = 0;
			while(i < Size)
				if(Buff[i] == ';')
				{
					frameIsFull = true;
					break;
				}else{
					i++;
				}
					
			if(frameIsFull)
			{
				float tmp_cell[5];
				if(sscanf((char*)Buff + 1, "%d:%f|%f|%f|%f|%f|;", &acb, &tmp_cell[0], &tmp_cell[1], &tmp_cell[2], &tmp_cell[3], &tmp_cell[4]) == 6)
				{
					removeBegin(i + 1);
					voltage[acb] = 0;
					for(int j = 0; j < 5; ++j)
					{
						cell[acb][j] = tmp_cell[j];
						voltage[acb] += cell[acb][j];
					}
					if(voltage[acb] < 1.0)
					{
						voltage[acb] = 0.0;
						plugged[acb] = false;
					}else{
						plugged[acb] = true;
						if(voltage[acb] < 17.5)
							printf("\033[31mBattery %d: %.2f;\033[0m\n", acb, voltage[acb]);
					}
					//cout << "Size == " << Size << endl;
					return true;
				}else{
					removeBegin(1);
				}
			}else{
				removeBegin(i + 1);
				return false;
			}
		}else{
			removeBegin(1);
		}
	}
	return false;
}

void Spark::getBatteries()
{
	unsigned char command[1];
	command[0] = ACB;
	writeData(command, 1);
}

int Spark::percent(int index)
{
	return (voltage[index] - 16.0) / 5.0 * 100.0;
}
