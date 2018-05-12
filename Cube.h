#pragma once

#include<stdlib.h>
#include<time.h>
// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

class Cube
{
	glm::vec3 pos;
	glm::vec3 foundExit;
	glm::vec3 direction;
	bool received;
	float angle;
	bool checked1, checked2;
	glm::vec3 ex2_2;
	glm::vec3 ex2_1;
public:
	glm::vec3 getPos();
	void getMsg(glm::vec3);
	void sendMsg(Cube&,glm::vec3);
	float rotation;
	bool isRotate;
	bool canReceive;
	glm::vec3 crossPerimiter;
	int colour;
	float d;

	bool discovered(glm::vec3);
	void makeStep();
	void setDir(glm::vec3);
	Cube(float,bool,float);
	~Cube();
};