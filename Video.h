#ifndef VIDEO_H
#define VIDEO_H

#include <iostream>
#include <stdio.h>
#include <ctime>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

class Video
{

public:
	vector<uchar> buff;//buffer for coding

	Video();
	Video(int camera) : Camera(camera) {};
	~Video();
	int OpenCamera(int camera = 0);
	int GetFrame(uint8_t*& jpegBuff, bool best_quality = false, bool sendToServer = false);
	int CloseCamera();

	void CreateWindow();
	void ShowFrame();
	void FindBanana();
	void SaveFrame();

private:
	vector<int> param;
	VideoCapture cap;
	Mat image;

	int Camera;
};

#endif
		

