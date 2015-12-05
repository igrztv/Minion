#ifndef LIDAR_H
#define LIDAR_H

#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
typedef unsigned char	   BYTE;
#endif

#include <vector>
#include <iostream>

#include "SerialPort.h"
#include "gameObject.h"

using namespace std;

#define MARKER 0xFA
#define DegreeInRads 0.01745329252
#define RadInDegrees 57.2957795131

class Lidar : public Serial
{
private:	
	int setAngle; //óãîë óñòàíîâêè ëèäàðà íà ïëàòôîðìå. Íóæåí äëÿ ñîâìåùåíèÿ íóëåé ëèäàðà è ðîáîòà

	struct Object
	{
		int length; //øèðèíà îáúåêòà â ìèëëèìåòðàõ
		int begin; // ïåðâàÿ òî÷êà îáúåêòà
		int end; // ïîñëåäíÿÿ òî÷êà îáúåêòà
		int pointsNum; //êîëè÷åñòâî òî÷åê, èç êîòîðûõ ñîñòîèò îáúåêò
		float correlation;
		bool isStand;
		vector<Point> points;
	};

public:

	vector<Object> object;

	struct LIDAR_SAMPLE
	{
		WORD Distance;
		WORD Distance_new;
		bool Warning;
		WORD Strength;
		DWORD Time;
		int x;
		int y;
		bool valid;
	};

	Lidar();
	~Lidar();

	void Process();
	bool CheckPack();
	void ReadPack();

	void ConvertToDecart(int id);
	void segregate_areas();

	float sin_arr[360], cos_arr[360];
	LIDAR_SAMPLE Samples[360];
	GameObject::MotionPoint position; //Absolute coordinates and angle

	char Error[30];

};

#endif
