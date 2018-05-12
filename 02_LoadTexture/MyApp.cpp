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

	// gener�ljunk egy text�ra er�forr�s nevet
    glGenTextures(1, &tmpID);
	// aktiv�ljuk a most gener�lt nev� text�r�t
    glBindTexture(GL_TEXTURE_2D, tmpID);
	// t�lts�k fel adatokkal az...
    gluBuild2DMipmaps(  GL_TEXTURE_2D,	// akt�v 2D text�r�t
						GL_RGB8,		// a v�r�s, z�ld �s k�k csatorn�kat 8-8 biten t�rolja a text�ra
						256, 256,		// 256x256 m�ret� legyen
						GL_RGB,				// a text�ra forr�sa RGB �rt�keket t�rol, ilyen sorrendben
						GL_UNSIGNED_BYTE,	// egy-egy sz�nkopmonenst egy unsigned byte-r�l kell olvasni
						tex);				// �s a text�ra adatait a rendszermem�ria ezen szeglet�b�l t�lts�k fel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// biline�ris sz�r�s kicsiny�t�skor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// �s nagy�t�skor is
	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
}


bool CMyApp::Init()
{
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)
	glCullFace(GL_BACK); // GL_BACK: a kamer�t�l "elfel�" n�z� lapok, GL_FRONT: a kamera fel� n�z� lapok

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
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
											// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(vert),		// ennyi b�jt nagys�gban
		vert,	// err�l a rendszermem�riabeli c�mr�l olvasva
		GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat


							// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	);

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer l�trehoz�sa
	glGenBuffers(1, &m_ibID);
	// a VAO �szreveszi, hogy bind-olunk egy index puffert �s feljegyzi, hogy melyik volt ez!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

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
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(ex1_m_vaoID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &ex1_m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, ex1_m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
											// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(ex1),		// ennyi b�jt nagys�gban
		ex1,	// err�l a rendszermem�riabeli c�mr�l olvasva
		GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat


							// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	);

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer l�trehoz�sa
	glGenBuffers(1, &ex1_m_ibID);
	// a VAO �szreveszi, hogy bind-olunk egy index puffert �s feljegyzi, hogy melyik volt ez!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ex1_m_ibID);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

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
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(red_m_vaoID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &red_m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, red_m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
											// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(redCube),		// ennyi b�jt nagys�gban
		redCube,	// err�l a rendszermem�riabeli c�mr�l olvasva
		GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat


							// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	);

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer l�trehoz�sa
	glGenBuffers(1, &red_m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, red_m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInd), cubeInd, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

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
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(blue_m_vaoID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &blue_m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, blue_m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
												// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(blueCube),		// ennyi b�jt nagys�gban
		blueCube,	// err�l a rendszermem�riabeli c�mr�l olvasva
		GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat


							// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	);

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer l�trehoz�sa
	glGenBuffers(1, &blue_m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blue_m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInd), cubeInd, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

	//
	// shaderek bet�lt�se
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket t�rol� program l�trehoz�sa
	m_programID = glCreateProgram();

	// adjuk hozz� a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attrib�tumok hozz�rendel�se a shader v�ltoz�khoz
	// FONTOS: linkel�s el�tt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonos�t�ja, amib�l egy v�ltoz�hoz szeretn�nk hozz�rendel�st csin�lni
							0,				// a VAO-beli azonos�t� index
							"vs_in_pos");	// a shader-beli v�ltoz�n�v
	glBindAttribLocation( m_programID, 1, "vs_in_col");
	glBindAttribLocation( m_programID, 2, "vs_in_tex0");

	// illessz�k �ssze a shadereket (kimen�-bemen� v�ltoz�k �sszerendel�se stb.)
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

		std::cout << "[app.Init()] S�der Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// egy�b inicializ�l�s
	//

	// vet�t�si m�trix l�trehoz�sa
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// shader-beli transzform�ci�s m�trixok c�m�nek lek�rdez�se
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
	// n�zeti transzform�ci� be�ll�t�sa
	m_matView = glm::lookAt(glm::vec3( 0,  0,  5),		// honnan n�zz�k a sz�nteret
							glm::vec3( 0,  0,  0),		// a sz�nt�r melyik pontj�t n�zz�k
							glm::vec3( 0,  1,  0));		// felfel� mutat� ir�ny a vil�gban

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
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram(m_programID);

	// majd k�ldj�k �t a megfelel� m�trixot!
	glUniformMatrix4fv(m_loc_view, 1, GL_FALSE, &(m_matView[0][0]));
	glUniformMatrix4fv(m_loc_proj, 1, GL_FALSE, &(m_matProj[0][0]));

	//kocka vao
	glBindVertexArray(red_m_vaoID);

	m_matWorld = glm::translate<float>(red.getPos().x, red.getPos().y, red.getPos().z);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	// kirajzol�s - red
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	glBindVertexArray(blue_m_vaoID);

	m_matWorld = glm::translate<float>(blue.getPos().x,blue.getPos().y,blue.getPos().z);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	// kirajzol�s - blue
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);

}

void CMyApp::RenderDisc()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// majd k�ldj�k �t a megfelel� m�trixot!
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
	// kirajzol�s - exit1
	glDrawArrays(GL_TRIANGLES, 0, 3);					

	m_matWorld = glm::translate<float>(exit2);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	// kirajzol�s - exit2
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

// a k�t param�terbe az �j ablakm�ret sz�less�ge (_w) �s magass�ga (_h) tal�lhat�
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}