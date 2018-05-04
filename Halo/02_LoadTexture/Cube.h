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

public:
	glm::vec3 getPos();
	void getMsg(glm::vec3);
	void sendMsg(Cube&);

	void makeStep();
	void setDir(glm::vec3);
	Cube(float,bool);
	~Cube();
};