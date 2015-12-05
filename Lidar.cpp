#include "Lidar.h"

Lidar::Lidar()
{
	position.angle = 0;
	position.location = Point(0,0);
	setAngle = 140;

	for(int i = 0; i < 360; i++){
		sin_arr[i] = sin(i*DegreeInRads);
		cos_arr[i] = cos(i*DegreeInRads);
	}

	LIDAR_SAMPLE ls = {0xFFFF, false, 0, 0};

	for(int i=0; i<360; i++)
		Samples[i] = ls;

}

Lidar::~Lidar()
{
	closePort();
}

void Lidar::Process()
{
	readData();
	while( Size >= 22 )
	{
		if(!CheckPack())
		{
			removeBegin(1);
			continue;
		}
		ReadPack();
		removeBegin(22);
	}

	Samples[0].valid = true;
	Samples[0].Distance = 100;
	ConvertToDecart(0);
}

bool Lidar::CheckPack()
{
	if( Buff[0] != MARKER )
		return false;
	WORD *p = (WORD*)Buff;
	DWORD sum32 = 0;
	for(int i=0; i<10; i++)
		sum32 = (sum32 << 1) + p[i];
	WORD sum = (sum32 & 0x7FFF) + (sum32 >> 15);
	sum &= 0x7FFF;
	return sum == p[10];
}

void Lidar::ReadPack()
{
	BYTE ind = Buff[1] - 0xA0;
	WORD Speed = Buff[2] | Buff[3] << 8;
	for(BYTE num=0; num<4; num++)
	{
		BYTE *p = Buff + 4 + num*4;
		WORD Distance;
		( p[1] & 0x80 ) ? Distance = 0xFFFF : Distance = ((WORD*)p)[0] & 0x3FFF;
		WORD Warning = p[1] & 0x40;
		WORD Strength = ((WORD*)p)[1];
		WORD id = ind*4+num + setAngle;
		id = id % 360;

		Samples[id].Distance = Distance;
		Samples[id].Warning = Warning;
		Samples[id].Strength = Strength;
		//Samples[id].Time = GetTickCount();
		Samples[id].valid = (Samples[id].Distance != 0xFFFF) && (!Samples[id].Warning);

		ConvertToDecart(id);
	}
}

void Lidar::ConvertToDecart(int id){	
	if( Samples[id].valid ){
		int i = (int)(position.angle * RadInDegrees + id) % 360;
		if(i < 0) i += 360;
		Samples[id].x = Samples[id].Distance * sin_arr[i] + position.location.x;
		Samples[id].y = Samples[id].Distance * cos_arr[i] + position.location.y;
	}
}

void Lidar::segregate_areas()
{
	
	object.erase(object.begin(),object.end());
	Object tmp;
	bool discard_capture;

	bool object_captured = false;
	for(int i = 0, a = 359; i < 360; a = i++)
	{
		discard_capture = true;		

		if(Samples[i].valid && Samples[a].valid)
		{			
			if(abs(Samples[i].Distance - Samples[a].Distance) < 50)
			{
				discard_capture = false;

				if(object_captured == false)//åñëè ìû íå ñîáèðàëè îáúåêò èç òî÷åê
				{
					tmp.begin = a;//íà÷èíàåì ñîáèðàòü íîâûé îáúåêò
					tmp.points.push_back(Point(Samples[a].x, Samples[a].y));
					//tmp.points.push_back(Samples[a].Distance);
				}
				object_captured = true;
				tmp.end = i;
				tmp.points.push_back(Point(Samples[i].x, Samples[i].y));
				//tmp.points.push_back(Samples[i].Distance);
			}
		}
		if(discard_capture)//åñëè ïðèñîåäèíåíèå íîâîé òî÷êè ê îáúåêòó ïðåðâàëîñü
		{
			if(object_captured == true)//åñëè ïðè ýòî ìû ñîáèðàëè îáúåêò èç òî÷åê, çàêàí÷èâàåì
			{
				tmp.length = GameObject::length(Samples[tmp.begin].x, Samples[tmp.begin].y, Samples[tmp.end].x, Samples[tmp.end].y);
				tmp.pointsNum = tmp.end - tmp.begin + 1;
				object.push_back(tmp);
				tmp.points.erase(tmp.points.begin(), tmp.points.end());//î÷èùàåì ìåñòî äëÿ ñëåäóþùåãî îáúåêòà
			}
			object_captured = false;
		}
	}
	if(!discard_capture)//çàêîí÷èëè ïðîõîä ïî âñåì òî÷êàì, à ïîñëåäíèé îáúåêò íå çàâåðøåí
	{
		if(object_captured)//åñëè ìû åãî ñîáèðàëè
		{
			if(object[0].begin == 359)//åñëè ïîñëåäíèé îáúåêò èìååò ñ ïåðâûì îáùóþ òî÷êó
			{
				//ýòîò îáúåêò ñâÿçûâàåì ñ ïåðâûì
				tmp.end = object[0].end;
				for (vector<Point>::iterator it = object[0].points.begin(); it != object[0].points.end(); ++it)
				//for (vector<int>::iterator it = object[0].points.begin(); it != object[0].points.end(); ++it)
					tmp.points.push_back(*it);				
				tmp.pointsNum = 360 + tmp.end - tmp.begin + 1;
				tmp.length = GameObject::length(Samples[tmp.begin].x, Samples[tmp.begin].y, Samples[tmp.end].x, Samples[tmp.end].y);
				object[0] = tmp;
			}
		}
	}

	//printf("objects == %d\n", object.size());

}
