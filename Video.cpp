#include "Video.h"

Video::Video()
{
	Camera = 0;
	param = vector<int>(2);
	param[0] = CV_IMWRITE_JPEG_QUALITY;
	//param[1] = 27;
}
	
int Video::OpenCamera(int camera)
{
	if(!cap.isOpened())
	{
		cap.open(camera);
		if(!cap.isOpened())
			return -1;
		Camera = camera;
	}
}

int Video::CloseCamera()
{
	cap.release();
}

int Video::GetFrame(uint8_t*& jpegBuff, bool best_quality, bool sendToServer)
{
	if(!OpenCamera(Camera))
		return -1;

	cap >> image; // get a new frame from camera
	if(best_quality)
	{
		param[1] = 100;
		time_t t = time(0);  // t is an integer type
			cout << t << " seconds since 01-Jan-1970\n";
		char buf[100];
		sprintf(buf, "images/%lld.jpeg", (long long) t);
		imwrite(buf, image, param);
		if(sendToServer)
		{
			sprintf(buf, "./ftp.sh images/%lld.jpeg", (long long) t);
			system(buf);
		}
		return 0;
	}else{
		param[1] = 27;
		imencode(".jpg", image, buff, param);
		jpegBuff = &buff[0];
		return buff.size();
	}	
}

void Video::CreateWindow()
{
	namedWindow("frame");
	
}

void Video::ShowFrame()
{
	if(!OpenCamera(Camera))
	{
		cout << "no openned cameras\n";
		return;
	}

	cap >> image; // get a new frame from camera
	imshow("frame", image);
}

void Video::FindBanana()
{
	if(!OpenCamera(Camera))
		return;

	cap >> image; // get a new frame from camera
	Mat hsv;
	cvtColor(image, hsv, CV_BGR2HSV);
	inRange(hsv, Scalar(0, 150, 0), Scalar(100, 250, 150), image);
	imshow("frame", image);
}

void Video::SaveFrame()
{
	if(!OpenCamera(Camera))
		return;
	char buf[100];
	sprintf(buf, "images/%d.jpg", rand() % 100000);
	imwrite(buf, image);
}

Video::~Video()
{
	CloseCamera();
}

/*int send_video(uint8_t* jpegBuffer)
{
		int size = buff.size();
	//char* start = new char[8 + itoa(size)];
	uint8_t* sendFrame = new uint8_t[7 + buff.size() + 8];
	char start[8] = "<frame>";
	char stop[9] = "</frame>";
	memcpy(sendFrame, start, 7);
	memcpy(sendFrame+7, &buff[0], buff.size());
	memcpy(sendFrame + 7 + buff.size(), stop, 8);
	//cout << "sent: " << 
	client.writeData((char *)sendFrame, 7+buff.size()+8);
	// << endl;
	delete [] sendFrame;
	
	return 1;
}*/
