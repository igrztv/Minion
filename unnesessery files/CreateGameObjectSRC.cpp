const int NumberOfObjects = 21;
GameObject *objects[NumberOfObjects];

void createGameObjects()
{
	Scalar color(0,150,150);
	objects[0] = new Stand(Point(90,200), color);
	objects[1] = new Stand(Point(850,200), color);
	objects[2] = new Stand(Point(850,100), color);
	objects[3] = new Stand(Point(870,1355), color);
	objects[4] = new Stand(Point(90,1750), color);
	objects[5] = new Stand(Point(90,1850), color);
	objects[6] = new Stand(Point(1300,1400), color);
	objects[7] = new Stand(Point(1100,1770), color);
	color = Scalar(0,150,0);
	objects[8] = new Stand(Point(3000-90,200), color);
	objects[9] = new Stand(Point(3000-850,200), color);
	objects[10] = new Stand(Point(3000-850,100), color);
	objects[11] = new Stand(Point(3000-870,1355), color);
	objects[12] = new Stand(Point(3000-90,1750), color);
	objects[13] = new Stand(Point(3000-90,1850), color);
	objects[14] = new Stand(Point(3000-1300,1400), color);
	objects[15] = new Stand(Point(3000-1100,1770), color);

	objects[16] = new Cup(Point(1500,1650));
	objects[17] = new Cup(Point(250,1750));
	objects[18] = new Cup(Point(910,830));
	objects[19] = new Cup(Point(3000-250,1750));
	objects[20] = new Cup(Point(3000-910,830));
};

void destroyGameObjects()
{
	for(int i = 0; i < NumberOfObjects; ++i)
	{
		delete objects[i];
	}
}
	createGameObjects();
	while(true)
		reDraw(Minion, objects, NumberOfObjects);	
	destroyGameObjects();
