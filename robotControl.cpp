#include "robotControl.h"

Robot::Robot(Point start_point, int start_angle)
{
	position.location = start_point;
	position.angle = start_angle;
	previous_task_done = true;
	
	WheelBase = WHEELBASE;

	lidar.position = position;
	speed.koef = 1.0;
}

Robot::Robot(int start_x, int start_y, int start_angle)
{
	position.location.x = start_x;
	position.location.y = start_y;
	position.angle = start_angle;
	Position(position);
	previous_task_done = true;
	WheelBase = WHEELBASE;
	printData = false;
	speed.koef = 1.0;
	//gameObjects[0] = new Stand();
	//Stand::width = 1000;
	//gameObjects[1] = new Cup();
	//Cup::width = 100;
}

void Robot::Position(GameObject::MotionPoint setPosition)
{
	position = setPosition;
	lidar.position = position;
	//printf("Robot: (%d %d, %f)\n",position.location.x,position.location.y,position.angle);
}

GameObject::MotionPoint Robot::Position()
{
	return position;
}

void Robot::get_path(int path0, int path1)
{
	//printf("(%d %d) ", path0, path1);
	float path;
	if(path0 == path1)
	{
		path = path0 * TICKStoMM ;
	}else{
		float alpha = (path1 - path0) * TICKStoMM / (float)WheelBase;
		//Angle(Position().angle + alpha);
		position.angle += alpha;
		float centerRadius = (path0 + path1) * TICKStoMM / (alpha * 2.0);
		path = sin(alpha) * centerRadius;
	}
	position.location.x += path * cos(position.angle);
	position.location.y -= path * sin(position.angle);
	Position(position);
}

void Robot::Angle(float setAngle)
{
	position.angle = setAngle;
	Position(position);
}

void Robot::move_manual(bool mode)
{
	byte command[1] = {0};
	if(mode == MANUAL)
		command[0] = MOVE_MANUAL;
	else
		command[0] = MOVE_AUTO;
	
	writeData(command, 1);
}

void Robot::move()
{
	move(speed.left, speed.right);
}

void Robot::move(int speed0, int speed1)
{
	byte command[5] = {MOVE, 0x00, 0x00, 0x00, 0x00};
	int tmp = abs(speed0);
	if(tmp > 255)
		command[1] = 0xFF;
	else
		command[1] = tmp & 0xFF;
	command[1] = command[1] * speed.koef;
	 
	if(speed0 == 0)
		if(speed.leftWheel > 0)
			command[2] = 200;
		else
			command[2] = 20;
	else
		if(speed0 >= 0)
			command[2] = 200;
		else
			command[2] = 20;

	tmp = abs(speed1);
	if(tmp > 255)
		command[3] = 0xFF;
	else
		command[3] = tmp & 0xFF;
	command[3] = command[3] * speed.koef;
		
	if(speed1 == 0)
		if(speed.rightWheel > 0)
			command[4] = 200;
		else
			command[4] = 20;
	else
		if(speed1 >= 0)
			command[4] = 200;
		else
			command[4] = 20;

	writeData(command, 5);
	
	speed.leftWheel = speed0;
	speed.rightWheel = speed1;
}

void Robot::stop()
{
	byte command[5] = {MOVE, 0x00, 0x00, 0x00, 0x00};
	writeData(command, 5);
}

void Robot::path(int forthcoming_distance)
{
	//printf("path: %d\n", forthcoming_distance);

	byte command[3] = {PATH, 0x00, 0x00};
	command[1] = (forthcoming_distance & 0xFF00) >> 8;
	command[2] = forthcoming_distance & 0x00FF;
	writeData(command, 3);
}

void Robot::skipSettings()
{
	byte command[1] = {SKIP_SETTINGS};
	writeData(command, 1);
}

void Robot::getResponse()
{
	if(!isConnected()) return;

	int count = readData();

	while(Size > 0)
	{

		bool parsed = false;

		if(Buff[0] == GET_PATH)
		{
			int path0 = 0;
			int path1 = 0;
			if(Size >= 5)
			{
				path0 = Buff[1];
				path1 = Buff[4];
				if(Buff[2] == '-') path0 = -path0;
				if(Buff[3] == '-') path1 = -path1;			
				//if(path0 != 0) cout << " p0: " << path0;
				//if(path1 != 0) cout << " p1: " << path1;
				get_path(path0, path1);
				removeBegin(5);
				parsed = true;
			}
		}

		if(moveStage.size() > 0)//если есть текущие задания
		{

			if(Buff[0] == (STOPPED + 1) && !moveStage[0].done1)
			{
				printf("STOPPED %x\n", STOPPED + 1);
				moveStage[0].done1 = true;
				removeBegin(1);
				parsed = true;
			}
			if(Buff[0] == STOPPED && !moveStage[0].done0)
			{
				printf("STOPPED %x\n", STOPPED );
				moveStage[0].done0 = true;
				removeBegin(1);
				parsed = true;
			}
			//if(moveStage[0].done0 && moveStage[0].done1)
			//	printf("Robot: (%d %d, %f)\n",position.location.x,position.location.y,position.angle);
		}

		if(parsed == false)
		{
			if(Size > 0)
			{
				//printf("%x ", Buff[0]);
				//cout << " remove\n";
				removeBegin(1);
			}
		}
	}
}

void Robot::makeTrack(float turnAngle)
{

	int length = 0;
	int spd0 = 0;
	int spd1 = 0;

	if(turnAngle != 0.0)
	{
		if(turnAngle >= PI) turnAngle -= 2*PI;
		if(turnAngle < -PI) turnAngle += 2*PI;
		length = abs(turnAngle) * WheelBase / 2.0;
		spd0 = (turnAngle > 0) ? -255 : 255;
		spd1 = (turnAngle > 0) ? 255 : -255;
	}else{
		int size = trackPoints.size();
		if(size > 1)
		{
			int i = size - 1;
			int dx = trackPoints[i].location.x - trackPoints[i-1].location.x;
			int dy = trackPoints[i].location.y - trackPoints[i-1].location.y;

			float angle;
			if(dx == 0)
			{
				(dy > 0) ? angle = 3*PI/2.0 : angle = PI/2.0;
			}else{
				angle = -atan(dy / (float)dx);
				if(dx < 0){
					(dy > 0) ? angle -= PI : angle += PI;
				}
				if(angle < 0) angle += 2*PI;
			}			

			trackPoints[i].angle = angle;
			//printf("angle = %f\n", angle);

			if(trackPoints[i].angle != trackPoints[i-1].angle)
			{
				makeTrack(trackPoints[i].angle - trackPoints[i-1].angle);
			}

			length = sqrt((float)(dx * dx + dy * dy)); //[mm]
			spd0 = 255;
			spd1 = 255;
		}
	}
	if(trackPoints.size() > 1)
	{
		int last = trackPoints.size() - 1;
		int lengthTICKS = length * MMtoTICKS; //[TICKS]
		MoveStage tmp = {
			spd0,
			spd1,
			lengthTICKS,
			lengthTICKS,
			false,
			false,
			trackPoints[last]
		};

		moveStage.push_back(tmp);
		last = moveStage.size() - 1;
		printf("%d: angle(%f), path(%d:%d)\n",last + 1,turnAngle,moveStage[last].forthcoming_distance0,moveStage[last].forthcoming_distance1);
	}

}

void Robot::sendCommands()
{
	if(moveStage.size() > 0)//если есть задания
	{
		if(moveStage[0].done1 & moveStage[0].done0)//если задание завершено (колеса доехали)
		{
			moveStage.erase(moveStage.begin());//удаляем из очереди
			previous_task_done = true;//говорим, что можно давать следующее задание			
		}
		if(previous_task_done)//если предыдущее задание выполнено
		{
			if(moveStage.size() > 0)////если есть еще задания
			{
				path(moveStage[0].forthcoming_distance0);//задаем путь
				move(moveStage[0].speed0,moveStage[0].speed1);//задаем скорость
				previous_task_done = false;//выставляем флаг ожидания завершения задания
			}
		}
	}
}

void Robot::correlation()
{
	for(int i = 0; i < lidar.object.size(); i++)//для каждого найденного участка (предполагаемого объекта)
	{
		//lidar.object[i].points.push_back(lidar.position.location);
		lidar.object[i].correlation = matchShapes(lidar.object[i].points, Stand::pattern, CV_CONTOURS_MATCH_I3, 0.0);
	}
}

/*void Robot::MoveByPolar(int S, int T)
{
	int step = 5;
	if(S == 0)
		if(speedLevel > 0)
			speedLevel -= step;
		else
			speedLevel += step;
	else
		if(S > 0)
			speedLevel += step;
		else
			speedLevel -= step;
			
	if(speedLevel > 255) speedLevel = 255;
	if(speedLevel < -255) speedLevel = -255;
	
	if(S > 0)
		speedLevel = 255;
	else
		speedLevel = -255;
		
	float len = sqrt(S*S + T*T);
	if(len == 0)
	{
		move(0, 0);
		return;
	}
	float cos = T / len;
	if(len > 1.0)
		len = 1.0;
	if(S > 0)
		move( (1 + cos) * len * speedLevel, (1 - cos) * len * speedLevel );
	else
		move( (1 + cos) * len * speedLevel , (1 - cos) * len * speedLevel );
	
}*/

void Robot::MoveByPolar(int X, int Y)
{
	//temporary hardcode for descrete changing values of speed
	//rewrite for continious values (smoot changing)
	if(X == 0){ speed.forward = false; speed.backward = false;}
	if(Y == 0){ speed.left = false; speed.right = false;}
	if(X == 1){ speed.forward = true; speed.backward = false;}
	if(X == -1){ speed.forward = false; speed.backward = true;}
	if(Y == 1){ speed.left = false; speed.right = true;}
	if(Y == -1){ speed.left = true; speed.right = false;}
	if(speed.forward)
	{
		if(speed.right)
		{
			move(255, 127);
			return;
		}
		if(speed.left)
		{
			move(127, 255);
			return;
		}
		move(255, 255);
		return;
	}
	if(speed.backward)
	{
		if(speed.left)
		{
			move(-127, -255);
			return;
		}
		if(speed.right)
		{
			move(-255, -127);
			return;
		}
		move(-255, -255);
		return;
	}
	if(speed.left)
	{
		move(-80, 80);
		return;
	}
	if(speed.right)
	{
		move(80, -80);
		return;
	}
		
	move(0, 0);		
	
}
