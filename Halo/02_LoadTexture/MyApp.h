#pragma once

#include<stdlib.h>
#include<time.h>
#include "Cube.h"
// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

const int N = 44;

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	float r;

	bool Init();
	void Clean();

	void Update(Cube&,Cube&);
	void Render(glm::vec3,glm::vec3);
	void RenderDisc();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);

	bool isExit(glm::vec3);

protected:
	// bels� elj�r�sok
	GLuint GenTexture();

	// shaderekhez sz�ks�ges v�ltoz�k
	GLuint m_programID; // shaderek programja

	// transzform�ci�s m�trixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// m�trixok helye a shaderekben
	GLuint	m_loc_world;
	GLuint	m_loc_view;
	GLuint	m_loc_proj;

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object er�forr�s azonos�t�
	GLuint m_vboID; // vertex buffer object er�forr�s azonos�t�
	GLuint m_ibID;  // index buffer object er�forr�s azonos�t�

	GLuint ex1_m_vaoID; // vertex array object er�forr�s azonos�t�
	GLuint ex1_m_vboID; // vertex buffer object er�forr�s azonos�t�
	GLuint ex1_m_ibID;  // index buffer object er�forr�s azonos�t�

	GLuint red_m_vaoID; // vertex array object er�forr�s azonos�t�
	GLuint red_m_vboID; // vertex buffer object er�forr�s azonos�t�
	GLuint red_m_ibID;  // index buffer object er�forr�s azonos�t�

	GLuint blue_m_vaoID; // vertex array object er�forr�s azonos�t�
	GLuint blue_m_vboID; // vertex buffer object er�forr�s azonos�t�
	GLuint blue_m_ibID;  // index buffer object er�forr�s azonos�t�

	GLuint m_rndTextureID; // text�ra er�forr�s azonos�t�
	GLuint m_waterTextureID; // f�jlb�l bet�lt�tt text�ra azonos�t�ja

	struct Vertex
	{
		glm::vec3 p; // poz�ci�
		glm::vec3 c; // sz�n
	};

	float edge;
	float d;
	float exitAngle;
	glm::vec3 exit1;
	glm::vec3 exit2;
};

