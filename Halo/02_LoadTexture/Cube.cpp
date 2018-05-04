#pragma once
#include "Cube.h"
#include <iostream>

void Cube::sendMsg(Cube& to)
{
	to.getMsg(pos);
}

void Cube::getMsg(glm::vec3 pos_)
{
	std::cout << "MSG";
	received = true;
	foundExit = pos_;
}
	
void Cube::makeStep()
{
	if (received == true)
	{
	
		std::cout << "Received msg" << std::endl;
		received = false;
	}
	else if (pow((pos.x + direction.x * 0.02),2) + pow((pos.y + direction.y* 0.02),2) > 1)
	{
		direction = glm::vec3(0);
		std::cout << "Perimiter" << std::endl;
	}
	
	pos = pos + (direction * glm::vec3(0.02));
}

void Cube::setDir(glm::vec3 newDir)
{
	direction=newDir;
}

glm::vec3 Cube::getPos()
{
	return pos;
}

Cube::Cube(float angle_, bool colour): pos(glm::vec3(0,0,0)), received(false), angle(angle_)
{
	if (colour == true)
	{
		direction = glm::normalize(glm::vec3(cos(angle / 2), sin(angle / 2), 0));
	}
	else
	{
		direction = -glm::normalize(glm::vec3(cos(angle / 2), sin(angle / 2), 0));
	}
};

Cube::~Cube() {};