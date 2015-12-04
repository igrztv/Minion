#ifndef DRAWING_H
#define DRAWING_H

#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

#include "Lidar.h"
#include "robotControl.h"
#include "gameObject.h"

#define ROWS 600
#define COLS 800

#define Scale  4.0
#define justifyY  (ROWS - 2000 / Scale) / 2
#define justifyX  (COLS - 3000 / Scale) / 2

#define ROBOT_POS 0
#define STAND_POS 1
#define DRAW_TRACK 2

//Scale real millimeters to image pixels
inline Point r2i(Point p)
{
	p.x = p.x/Scale + justifyX;
	p.y = p.y/Scale + justifyY;
	return p;
}
//Scale real millimeters to image pixels
inline Point r2i(int x, int y)
{	
	return r2i(Point(x,y));
}
//Scale image pixels to real millimeters
inline Point i2r(int x, int y)
{
	return Point ((x-justifyX)*Scale, (x-justifyX)*Scale);
}

void init_windows(Robot &robot);
Mat draw_lidar_points(Lidar &lidar);
void draw_lidar_inLine(const Lidar &lidar);
static void onMouse(int event, int x, int y, int, void* );
Mat draw_movable_objets(GameObject **objects, int count);
void DRAW_POLYGON();
void reDraw(Robot &robot, GameObject **objects, const int count);

#endif