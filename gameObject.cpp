#include "gameObject.h"
//#include "drawing.h"

float GameObject::length(const Point p1, const Point p2)
{
	int dx = p1.x - p2.x;
	int dy = p1.y - p2.y;
	return sqrt((float)(dx * dx + dy * dy));
}

float GameObject::length(const int x1, const int y1, const int x2, const int y2)
{
	int dx = x1 - x2;
	int dy = y1 - y2;
	return sqrt((float)(dx * dx + dy * dy));
}

vector<Point> Stand::pattern;

vector<Point> Stand::Pattern()
{
	return pattern;
}

void Stand::Pattern(vector<Point> _pattern)
{
	pattern = _pattern;
}

void Stand::Location(Point _location)
{
	location = _location;
}

Point Stand::Location()
{
	return location;
}

void Cup::Location(Point _location)
{
	location = _location;
}

Point Cup::Location()
{
	return location;
}

bool Stand::isMovable = false;

void Stand::IsMovable(bool _isMovable)
{
	isMovable = _isMovable;
}

bool Stand::IsMovable()
{
	return isMovable;
}

bool Cup::isMovable = false;

void Cup::IsMovable(bool _isMovable)
{
	isMovable = _isMovable;
}

bool Cup::IsMovable()
{
	return isMovable;
}

vector<Point> Cup::Pattern()
{
	return pattern;
}

vector<Point> Cup::pattern;

void Cup::Pattern(vector<Point> _pattern)
{
	pattern = _pattern;
}

int Stand::width = 0;

void Stand::Width(int _width)
{
	width = _width;
}

int Stand::Width()
{
	return width;
}

int Cup::width = 0;

void Cup::Width(int _width)
{
	width = _width;
}

int Cup::Width()
{
	return width;
}

/*void Stand::Draw(Mat image)
{
	circle(image, r2i(location), 30/Scale, color, 1);
}

void Cup::Draw(Mat image)
{
	circle(image, r2i(location), 50/Scale, Scalar(100,100,100), 2);
}*/
