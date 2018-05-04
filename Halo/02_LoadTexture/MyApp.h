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
	// belsõ eljárások
	GLuint GenTexture();

	// shaderekhez szükséges változók
	GLuint m_programID; // shaderek programja

	// transzformációs mátrixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// mátrixok helye a shaderekben
	GLuint	m_loc_world;
	GLuint	m_loc_view;
	GLuint	m_loc_proj;

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító
	GLuint m_ibID;  // index buffer object erõforrás azonosító

	GLuint ex1_m_vaoID; // vertex array object erõforrás azonosító
	GLuint ex1_m_vboID; // vertex buffer object erõforrás azonosító
	GLuint ex1_m_ibID;  // index buffer object erõforrás azonosító

	GLuint red_m_vaoID; // vertex array object erõforrás azonosító
	GLuint red_m_vboID; // vertex buffer object erõforrás azonosító
	GLuint red_m_ibID;  // index buffer object erõforrás azonosító

	GLuint blue_m_vaoID; // vertex array object erõforrás azonosító
	GLuint blue_m_vboID; // vertex buffer object erõforrás azonosító
	GLuint blue_m_ibID;  // index buffer object erõforrás azonosító

	GLuint m_rndTextureID; // textúra erõforrás azonosító
	GLuint m_waterTextureID; // fájlból betöltött textúra azonosítója

	struct Vertex
	{
		glm::vec3 p; // pozíció
		glm::vec3 c; // szín
	};

	float edge;
	float d;
	float exitAngle;
	glm::vec3 exit1;
	glm::vec3 exit2;
};

