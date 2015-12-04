#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
using namespace std;

#define PI 3.14159
//degrees to radians
#define d2r(degree) degree * 0.017453
//radians to degrees
#define r2d(radian) radian * 57.2957795

class GameObject
{
public:

	GameObject() {/*printf("GameObject\n");*/};
	~GameObject() {/*printf("~GameObject\n");*/};

	struct MotionPoint
	{
		Point2f location;
		float angle;
	};

	typedef Point StaticPoint;

	static float length(const Point p1, const Point p2);
	static float length(const int x1, const int y1, const int x2, const int y2);

	virtual void Pattern(vector<Point> _pattern){};
	virtual vector<Point> Pattern(){vector<Point> p(0);return p;};
	virtual void Location(Point _location){};
	virtual Point Location(){return Point(-1,-1);};
	virtual void IsMovable(bool _isMovable){};
	virtual bool IsMovable(){return false;};
	virtual void Width(int _width){};
	virtual int Width(){ return -1;};
	virtual void Draw(Mat image){};

};

//class InnerCorner : public GameObject {};
//class OutterCorner : public GameObject {};

class Stand : public GameObject
{

public:

	Scalar color;

	Stand() {printf("Stand\n");};
	Stand(Point position, Scalar c) {location = position; color = c;};
	~Stand() {printf("~Stand\n");};

	static vector<Point> pattern;
	void Pattern(vector<Point> _pattern);
	vector<Point> Pattern();

	Point location;
	void Location(Point _location);
	Point Location();

	static bool isMovable;
	void IsMovable(bool _isMovable);
	bool IsMovable();

	static int width;
	void Width(int _width);
	int Width();

	//void Draw(Mat image);

};

class Cup : public GameObject
{

public:

	Cup() {printf("Cup\n");};
	Cup(Point position) {location = position;};
	~Cup() {printf("~Cup\n");};

	static vector<Point> pattern;
	void Pattern(vector<Point> _pattern);
	vector<Point> Pattern();

	Point location;
	void Location(Point _location);
	Point Location();

	static bool isMovable;
	void IsMovable(bool _isMovable);
	bool IsMovable();

	static int width;
	void Width(int _width);
	int Width();

	//void Draw(Mat image);
};

#endif
