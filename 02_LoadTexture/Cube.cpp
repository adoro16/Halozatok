#pragma once
#include "Cube.h"
#include <iostream>

void Cube::sendMsg(Cube& to,glm::vec3 ex)
{
	to.getMsg(ex);
}

void Cube::getMsg(glm::vec3 pos_)
{
	std::cout << "MSG";
	received = true;
	foundExit = pos_;
	canReceive = false;
}
	
bool Cube::discovered(glm::vec3 ex)
{
	glm::vec3 norm = glm::vec3(pos.y - crossPerimiter.y, crossPerimiter.x - pos.x, 0);
	if (colour == 1)
	{
		return ((norm.x*ex.x + norm.y*ex.y)  < (norm.x*pos.x + norm.y*pos.y));
	}
	else
	{
		return ((norm.x*ex.x + norm.y*ex.y ) > (norm.x*pos.x + norm.y*pos.y));
	}
}

void Cube::makeStep()
{
	if (received == true)
	{
		isRotate = false;
		received = false;
		ex2_1 = glm::mat3(cos(d), -sin(d), 0, sin(d), cos(d), 0, 0, 0, 1)*foundExit;
		ex2_2 = glm::mat3(cos(-d), -sin(-d), 0, sin(-d), cos(-d), 0, 0, 0, 1)*foundExit;
		if (discovered(ex2_1) )
		{
			std::cout << "discovered ex2_1 "<<colour<<" : " << ex2_1.x <<"."<< ex2_1.y << std::endl;
			if (glm::distance(pos, foundExit) < glm::distance(pos, ex2_2))
			{
				direction = glm::vec3(foundExit.x - pos.x, foundExit.y - pos.y, 0);
			}
			else
			{
				direction = glm::vec3(ex2_2.x - pos.x, ex2_2.y - pos.y, 0);
				checked2 = true;
			}
		}
		else if (discovered(ex2_2) )
		{
			std::cout << "discovered ex2_2 " << colour << " : " << ex2_2.x <<"."<< ex2_2.y << std::endl;
			if (glm::distance(pos, foundExit) < glm::distance(pos, ex2_1))
			{
				direction = glm::vec3(foundExit.x - pos.x, foundExit.y - pos.y, 0);
			}
			else
			{
				direction = glm::vec3(ex2_1.x - pos.x, ex2_1.y - pos.y, 0);
				checked1=true;
			}
		}
		else
		{
			std::cout << "undiscovered " << colour << " : " << std::endl;
			if (glm::distance(pos, foundExit) < glm::distance(pos, ex2_2))
			{
				if (glm::distance(pos, foundExit) < glm::distance(pos, ex2_1))
				{
					direction = glm::vec3(foundExit.x - pos.x, foundExit.y - pos.y, 0);
				}
				else
				{
					direction = glm::vec3(ex2_1.x - pos.x, ex2_1.y - pos.y, 0);
					checked1 = true;
				}
			}
			else
			{
				if (glm::distance(pos, ex2_2) < glm::distance(pos, ex2_1))
				{
					direction = glm::vec3(ex2_2.x - pos.x, ex2_2.y - pos.y, 0);
					checked2 = true;
				}
				else
				{
					direction = glm::vec3(ex2_1.x - pos.x, ex2_1.y - pos.y, 0);
					checked1 = true;
				}
			}		
		}
	
	}
	else if (crossPerimiter==glm::vec3(0) && (pow((pos.x + direction.x * 0.02),2) + pow((pos.y + direction.y* 0.02),2) > 1))
	{
		direction = glm::vec3(0);
		std::cout << "Perimiter "<<colour<< " : " <<pos.x<<"."<<pos.y<< std::endl;
		crossPerimiter = pos;
		isRotate = true;
		rotation = colour*((2 * M_PI) / 360);
	}
	
	pos = pos + (direction * glm::vec3(0.02));

	if (checked1 && (glm::distance(pos, ex2_1) < 0.05))
	{
		direction = glm::vec3(ex2_2.x - pos.x, ex2_2.y - pos.y, 0);
		checked2 = true;
	}
	else if (checked2 && (glm::distance(pos, ex2_1) < 0.05))
	{
		direction = glm::vec3(ex2_1.x - pos.x, ex2_1.y - pos.y, 0);
		checked1 = true;
	}

	if (isRotate)
	{
		pos=glm::mat3(cos(rotation), -sin(rotation), 0, sin(rotation), cos(rotation), 0, 0, 0, 1)*pos;
	}
}

void Cube::setDir(glm::vec3 newDir)
{
	direction=newDir;
}

glm::vec3 Cube::getPos()
{
	return pos;
}

Cube::Cube(float angle_, bool colour_, float dist): pos(glm::vec3(0,0,0)), d(dist), received(false), angle(angle_), isRotate(false), crossPerimiter(glm::vec3(0)), rotation(0), canReceive(true), checked1(false),checked2(false)
{
	if (colour_ == true)
	{
		colour = -1;
		direction = glm::vec3(cos(angle / 2), sin(angle / 2), 0);
	}
	else
	{
		colour = 1;
		direction = glm::vec3(cos(-angle / 2), sin(-angle / 2), 0);
	}
};

Cube::~Cube() {};