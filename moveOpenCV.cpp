
static void onMouseMove(int event, int x, int y, int, void* a)
{
	if(event != EVENT_LBUTTONDOWN)
		return;

	//Robot*robot = (Robot*)Minion;
	cout << "X, Y = " << x-50 << " " << y-50 << endl;
	int moveX = ((x - 50) / 25.0);
	int moveY = ((y - 50) / 25.0);
	cout << "moveX, Y = " << moveX << " " << moveY << endl;
}

setMouseCallback("move", onMouse, &robot);


cv::namedWindow("move", cv::CV_WINDOW_NORMAL);
cv::resizeWindow("move", 100, 100);
Mat blank(100, 100, CV_8UC1, Scalar(0,0,0));
imshow("move", blank);

while(1)
{
	int key = cv::waitKey(30);
	switch(key)
	{
		case 'w': moveX = 1; break;
		case 'a': moveY = -1; break;
		case 's': moveX = -1; break;
		case 'd': moveY = 1; break;
		default: break;
	}
	Minion.MoveByPolar(moveX, moveY);
}

cv::destroyWindow("move");