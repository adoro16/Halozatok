#include "MyApp.h"
#include "GLUtils.hpp"

#include <GL/GLU.h>
#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
	m_rndTextureID = 0;
	m_waterTextureID = 0;
	edge = 0.1;
	r = 1;

	d = 0.8; // kijaratok tavolsaga
	exitAngle = d / r;
}


CMyApp::~CMyApp(void)
{
}

GLuint CMyApp::GenTexture()
{
    unsigned char tex[256][256][3];
 
    for (int i=0; i<256; ++i)
        for (int j=0; j<256; ++j)
        {
			tex[i][j][0] = rand()%256;
			tex[i][j][1] = rand()%256;
			tex[i][j][2] = rand()%256;
        }
 
	GLuint tmpID;

	// generáljunk egy textúra erõforrás nevet
    glGenTextures(1, &tmpID);
	// aktiváljuk a most generált nevû textúrát
    glBindTexture(GL_TEXTURE_2D, tmpID);
	// töltsük fel adatokkal az...
    gluBuild2DMipmaps(  GL_TEXTURE_2D,	// aktív 2D textúrát
						GL_RGB8,		// a vörös, zöld és kék csatornákat 8-8 biten tárolja a textúra
						256, 256,		// 256x256 méretû legyen
						GL_RGB,				// a textúra forrása RGB értékeket tárol, ilyen sorrendben
						GL_UNSIGNED_BYTE,	// egy-egy színkopmonenst egy unsigned byte-ról kell olvasni
						tex);				// és a textúra adatait a rendszermemória ezen szegletébõl töltsük fel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// bilineáris szûrés kicsinyítéskor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// és nagyításkor is
	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
}


bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	//
	// geometria letrehozasa
	//
	Vertex vert[N];
	GLushort indices[N];

	//vert[0] = { glm::vec3(0,0,0), glm::vec3(0,1,0) };
	//vert[N/2] = { glm::vec3(0,0,-0.0001), glm::vec3(0,1,0) };
	//indices[0] = 0;
	//indices[N/2] = N/2;
	for (int i = 0; i < N; ++i) {
		vert[i] = { glm::vec3(cos(2 * M_PI / (N-1)*i)*r,sin(2 * M_PI / (N-1)*i)*r,0), glm::vec3(0,1,0) };
		indices[i] = i;

		/*vert[i + N/2] = {
			glm::vec3(cos(2 * M_PI / 20.0*i)*0.5,-sin(2 * M_PI / 20.0*i)*0.5,-0.0001),
			glm::vec3(0,1,0),
		};
		indices[i + N/2] = i + N/2;*/
	}
	
	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
											// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(vert),		// ennyi bájt nagyságban
		vert,	// errõl a rendszermemóriabeli címrõl olvasva
		GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat


							// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	);

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_ibID);
	// a VAO észreveszi, hogy bind-olunk egy index puffert és feljegyzi, hogy melyik volt ez!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	int randExit = rand() % (N / 2 - 1) + 1;
	exit1 = vert[randExit].p;
	exit2 = glm::mat3(cos(exitAngle), -sin(exitAngle), 0, sin(exitAngle), cos(exitAngle), 0, 0, 0, 1)*exit1;
	Vertex ex1[] =
	{
		{ glm::vec3(0.15,0,0.01),glm::vec3(0,0,0) },
		{ glm::vec3(0,0.15,0.01),glm::vec3(0,0,0)},
		{ glm::vec3(0,0,0.01), glm::vec3(0,0,0) }
		 
	};

	glGenVertexArrays(1, &ex1_m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(ex1_m_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &ex1_m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, ex1_m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
											// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(ex1),		// ennyi bájt nagyságban
		ex1,	// errõl a rendszermemóriabeli címrõl olvasva
		GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat


							// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	);

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &ex1_m_ibID);
	// a VAO észreveszi, hogy bind-olunk egy index puffert és feljegyzi, hogy melyik volt ez!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ex1_m_ibID);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	Vertex redCube[] =
	{
		//          x,  y, z             R, G, B
		{ glm::vec3(-edge/2, -edge/2, edge), glm::vec3(1, 0, 0) },
		{ glm::vec3(edge / 2, -edge / 2, edge), glm::vec3(1, 0, 0) },
		{ glm::vec3(-edge / 2,  edge / 2, edge), glm::vec3(1, 0, 0) },
		{ glm::vec3(edge / 2,  edge / 2, edge), glm::vec3(1, 0, 0) },

		{ glm::vec3(-edge / 2, -edge / 2, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(edge / 2, -edge / 2, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(-edge / 2,  edge / 2, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(edge / 2,  edge / 2, 0), glm::vec3(1, 0, 0) },
	};

	// indexpuffer adatai
	GLushort cubeInd[] =
	{
		0,1,2,2,1,3,//elso lap
		1,5,3,5,7,3,//masodik lap
		4,6,7,4,7,5,
		0,2,4,4,2,6,
		2,3,6,6,3,7,
		0,4,1,1,4,5
	};

	glGenVertexArrays(1, &red_m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(red_m_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &red_m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, red_m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
											// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(redCube),		// ennyi bájt nagyságban
		redCube,	// errõl a rendszermemóriabeli címrõl olvasva
		GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat


							// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	);

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &red_m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, red_m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInd), cubeInd, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	Vertex blueCube[] =
	{
		//          x,  y, z             R, G, B
		{ glm::vec3(-edge / 2, -edge / 2, edge), glm::vec3(0, 0, 1) },
		{ glm::vec3(edge / 2, -edge / 2, edge), glm::vec3(0, 0, 1) },
		{ glm::vec3(-edge / 2,  edge / 2, edge), glm::vec3(0, 0, 1) },
		{ glm::vec3(edge / 2,  edge / 2, edge), glm::vec3(0, 0, 1) },

		{ glm::vec3(-edge / 2, -edge / 2, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(edge / 2, -edge / 2, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(-edge / 2,  edge / 2, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(edge / 2,  edge / 2, 0), glm::vec3(0, 0, 1) },
	};

	glGenVertexArrays(1, &blue_m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(blue_m_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &blue_m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, blue_m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
												// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(blueCube),		// ennyi bájt nagyságban
		blueCube,	// errõl a rendszermemóriabeli címrõl olvasva
		GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat


							// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	);

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &blue_m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blue_m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInd), cubeInd, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	//
	// shaderek betöltése
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attribútumok hozzárendelése a shader változókhoz
	// FONTOS: linkelés elõtt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonosítója, amibõl egy változóhoz szeretnénk hozzárendelést csinálni
							0,				// a VAO-beli azonosító index
							"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation( m_programID, 1, "vs_in_col");
	glBindAttribLocation( m_programID, 2, "vs_in_tex0");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// egyéb inicializálás
	//

	// vetítési mátrix létrehozása
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// shader-beli transzformációs mátrixok címének lekérdezése
	m_loc_world = glGetUniformLocation( m_programID, "world");
	m_loc_view = glGetUniformLocation( m_programID, "view");
	m_loc_proj = glGetUniformLocation( m_programID, "proj");

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

glm::vec3 CMyApp::isExit(glm::vec3 pos)
{
	if (glm::distance(pos,exit1) < 0.05 )
	{
		return exit1;
	}
	else if (glm::distance(pos, exit2) < 0.05)
	{
		return exit2;
	}
	else
	{
		return glm::vec3(0);
	}
}

void CMyApp::Update(Cube& red,Cube& blue)
{
	// nézeti transzformáció beállítása
	m_matView = glm::lookAt(glm::vec3( 0,  0,  5),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban

	red.makeStep();
	glm::vec3 tmp = isExit(red.getPos());
	if (tmp != glm::vec3(0))
	{
		red.setDir( glm::vec3(0));
		if (red.canReceive && blue.canReceive)
		{
			red.sendMsg(blue,tmp);
		}
		red.isRotate = false;
	}

	blue.makeStep();
	tmp = isExit(blue.getPos());
	if (tmp != glm::vec3(0))
	{
		blue.setDir(glm::vec3(0));
		if (red.canReceive && blue.canReceive)
		{
			blue.sendMsg(red,tmp);
		}
		blue.isRotate = false;
	}
}

void CMyApp::Render(Cube& red, Cube& blue)
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram(m_programID);

	// majd küldjük át a megfelelõ mátrixot!
	glUniformMatrix4fv(m_loc_view, 1, GL_FALSE, &(m_matView[0][0]));
	glUniformMatrix4fv(m_loc_proj, 1, GL_FALSE, &(m_matProj[0][0]));

	//kocka vao
	glBindVertexArray(red_m_vaoID);

	m_matWorld = glm::translate<float>(red.getPos().x, red.getPos().y, red.getPos().z);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	// kirajzolás - red
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	glBindVertexArray(blue_m_vaoID);

	m_matWorld = glm::translate<float>(blue.getPos().x,blue.getPos().y,blue.getPos().z);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	// kirajzolás - blue
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);

}

void CMyApp::RenderDisc()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// majd küldjük át a megfelelõ mátrixot!
	glUniformMatrix4fv( m_loc_view,	1, GL_FALSE,	&(m_matView[0][0]) );
	glUniformMatrix4fv( m_loc_proj,	1, GL_FALSE,	&(m_matProj[0][0]) );
	
	glBindVertexArray(m_vaoID);
	m_matWorld = glm::mat4(1.0f);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	glDrawElements(GL_TRIANGLE_FAN, N, GL_UNSIGNED_SHORT, 0);
	//glDrawElements(GL_TRIANGLE_FAN, 22, GL_UNSIGNED_SHORT, (void*)(22 * sizeof(GLushort)));
	
	glBindVertexArray(0);

	//ex1
	glBindVertexArray(ex1_m_vaoID);
	m_matWorld = glm::translate<float>(exit1);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));
	// kirajzolás - exit1
	glDrawArrays(GL_TRIANGLES, 0, 3);					

	m_matWorld = glm::translate<float>(exit2);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	// kirajzolás - exit2
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );

}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{

}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}