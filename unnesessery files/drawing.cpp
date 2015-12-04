#include "drawing.h"

int MouseAction = DRAW_TRACK;

Mat imgOriginal(ROWS,COLS,CV_8UC3, Scalar(0,0,0));
//Mat MapImage(ROWS, COLS, CV_8UC3, Scalar(0,0,0));
const char MapWindowName[] = "Map";
int correlation = 2;

#define map_points 8
int map_arr[map_points+1][2] = {{0,0},{967,0},{967,580},{2033,580},{2033,0},{3000,0},{3000,2000},{0,2000}};

void init_windows(Robot &robot)
{
		namedWindow(MapWindowName, 1);
		//namedWindow("lidar vision", 1);
		setMouseCallback(MapWindowName, onMouse, &robot);
		//createTrackbar("correlation","lidar vision",&correlation,100);
		DRAW_POLYGON();
}

Mat drawTrack(Robot& robot)
{
	Mat trackImage(ROWS, COLS, CV_8UC3, Scalar(0,0,0));
	int size = robot.moveStage.size();
	if(size > 0)
	{
		for(int i = 1; i < size; ++i)
		{
			line(
				trackImage, 
				r2i(robot.moveStage[i-1].position.location), 
				r2i(robot.moveStage[i].position.location), 
				Scalar(150,150,150), 
				2
			);
		}
		line(
			trackImage, 
			r2i(robot.Position().location), 
			r2i(robot.moveStage[0].position.location), 
			Scalar(100,100,250), 
			2
		);
	}
	return trackImage;
}

static void onMouse(int event, int x, int y, int, void* Minion)
{
	if(event != EVENT_LBUTTONDOWN)
		return;

	Robot*robot = (Robot*)Minion;

	if(MouseAction == DRAW_TRACK)
	{
		GameObject::MotionPoint tmp = {Point((x-justifyX)*Scale,(y-justifyY)*Scale), d2r(0)};
		robot->trackPoints.push_back(tmp);
		int size = robot->trackPoints.size();
		robot->makeTrack();
		if(size == 1)
		{
			//circle(MapImage, Point(x, y), 4, Scalar(190,190,190),-1);
			robot->Position(tmp);
		}
	}
}

Mat draw_lidar_points(Lidar &lidar)
{

	Mat LidarPointsImage(ROWS, COLS, CV_8UC3, Scalar(0,0,0));
	for(int i = 0; i < 360; i++)
	{
		if(lidar.Samples[i].valid)
		{
			line(LidarPointsImage, r2i(lidar.position.location), r2i(lidar.Samples[i].x, lidar.Samples[i].y), Scalar(0,0,150));
		}
	}

	//cout << endl;
	if(lidar.object.size() > 0)
	for(int i = 0; i < lidar.object.size(); i++)
	{
		Scalar color(rand()%250, rand()%250, rand()%250);

		int b = lidar.object[i].begin;
		int a = lidar.object[i].end;
		//cout << lidar.object[i].correlation << endl;
		if(lidar.object[i].correlation >= (double)correlation/2.0)
			color = Scalar(255,255,0);
		else{
			color = Scalar(255,0,0);
		}
		line(LidarPointsImage, r2i(lidar.Samples[b].x, lidar.Samples[b].y), r2i(lidar.Samples[a].x, lidar.Samples[a].y), color);
		
		//if(Minion.lidar.object[i].correlation > 5)
		//{
		//	circle(LidarPointsImage, 
		//		r2i((Minion.lidar.Samples[a].x + Minion.lidar.Samples[b].x) / 2, (Minion.lidar.Samples[a].y + Minion.lidar.Samples[b].y) / 2),
		//		8, color);
		//}
	}

	lidar.ConvertToDecart(90);
	line(LidarPointsImage, r2i(lidar.position.location), r2i(lidar.Samples[90].x, lidar.Samples[90].y), Scalar(0,0,150));

	return LidarPointsImage;
}

void draw_lidar_inLine(const Lidar &lidar)
{
	Mat LidarPointsImage(400, 720, CV_8UC3, Scalar(0,0,0));
	for(int i = 0; i < 360; i++)
	{
		if(lidar.Samples[i].valid)
		{
			line(
				LidarPointsImage, 
				Point(2*i,400), 
				Point(2*i,400-lidar.Samples[i].Distance), 
				Scalar(0,0,150), 
				1
			);
		}
	}
	imshow("lidar vision", LidarPointsImage);
}

Mat draw_movable_objets(GameObject **objects, const int count)
{
	Mat movableObjectsImage(ROWS, COLS, CV_8UC3, Scalar(0,0,0));
	for(int i = 0; i < count; i++)
	{
		objects[i]->Draw(movableObjectsImage);
		//if(Minion.lidar.object[i].correlation > 5)
		//{
		//	circle(LidarPointsImage, 
		//		r2i((Minion.lidar.Samples[a].x + Minion.lidar.Samples[b].x) / 2, (Minion.lidar.Samples[a].y + Minion.lidar.Samples[b].y) / 2),
		//		8, color);
		//}
	}
	return movableObjectsImage;
}

void DRAW_POLYGON(){
	for(int i = 0; i < map_points; i++){
		line(
			imgOriginal,
			r2i(map_arr[i][0],map_arr[i][1]),
			r2i(map_arr[i+1][0],map_arr[i+1][1]), 
			Scalar(255,0,0), 
			2
		);
	}
}

void reDraw(Robot &robot, GameObject **objects, const int count)
{
		imshow(MapWindowName, 
			imgOriginal
			+ draw_lidar_points(robot.lidar)
			//+ MapImage
			+ drawTrack(robot)
			+ draw_movable_objets(objects, count)
		);
		//draw_lidar_inLine(lidar);
}
