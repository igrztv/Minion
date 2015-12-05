#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#define MINION
#ifdef _WIN32
//#include "C:\MRGN\Arduino_Sketchbook\Fish_driver\SerialCodes.h"
#include "SerialCodes.h"
#else
#include "SerialCodes.h"
#endif
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "gameObject.h"
#include "SerialPort.h"
#include "Lidar.h"
#include "Video.h"

using namespace cv;

class Robot : public Serial
{
private:
	//Serial Port; //íå óäàëÿòü äî ïðîâåðêè ïîäêëþ÷åíèÿ ðîáîòà

	struct MoveStage
	{
		int speed0;
		int speed1;
		unsigned int forthcoming_distance0;
		unsigned int forthcoming_distance1;
		bool done0;
		bool done1;
		GameObject::MotionPoint position;
	};
	
	//bool connected;//íå óäàëÿòü äî ïðîâåðêè ïîäêëþ÷åíèÿ ðîáîòà

	GameObject::MotionPoint position;//current position of robot

public:

	struct Speed
	{
		int leftWheel;
		int rightWheel;
		bool forward;
		bool backward;
		bool left;
		bool right;
		float koef;
	};

	Speed speed;
	int speedLevel;

	vector<GameObject::MotionPoint> trackPoints;//points to reach
	vector<MoveStage> moveStage;//driving params to go from trackPoints[i-1] to trackPoints[i]
	bool previous_task_done;//flag

	int WheelBase;

	static const int MANUAL = 1;
	static const int AUTO = 2;

	Robot(Point start_point, int start_angle);
	Robot(int start_x, int start_y, int start_angle);
	void move();
	void move(int speed0, int speed1);
	void stop();
	void move_manual(bool mode);
	void speedTest();
	void path(int forthcoming_distance);
	void getPath();
	void settings();
	void skipSettings();
	void getResponse();
	void makeTrack(float turnAngle = 0.0);
	void sendCommands();
	void Position(GameObject::MotionPoint);
	GameObject::MotionPoint Position();
	void get_path(int path0, int path1);
	void Angle(float setAngle);
	void MoveByPolar(int X, int Y);

	//void connect(const char * portName, unsigned int baudRate);
	//void disconnect();
	//bool isConnected(){ return connected; };

	Video camera;
	Video selfie;
	Lidar lidar;
	//GameObject *gameObjects[2];
	void correlation();

};

#endif
