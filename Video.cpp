#include "Video.h"

Video::Video()
{
	Camera = 0;
	param = vector<int>(2);
	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 27;
}
	
int Video::OpenCamera(int camera)
{
	if(!cap.isOpened())
	{
		cap.open(camera);
		if(!cap.isOpened())
			return -1;
	}
}

int Video::CloseCamera()
{
	cap.release();
}

int Video::GetFrame(uint8_t*& jpegBuff)
{
	if(!OpenCamera(Camera))
		return -1;
	
	Mat image;
    cap >> image; // get a new frame from camera
    imencode(".jpg", image, buff, param);
    jpegBuff = &buff[0];
    return buff.size();
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
