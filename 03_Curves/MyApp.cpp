#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>

#include "imgui\imgui.h"

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
}


CMyApp::~CMyApp(void)
{
	
}

glm::vec3 CMyApp::GetPos(float u, float v)
{
	u *= 2 * 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;

	return glm::vec3(r*cu*sv, r*cv, r*su*sv);
}

glm::vec3 CMyApp::GetUV_henger(float u, float v) {
	float h = 1;
	float r = 0.5;
	u *= 2 * M_PI;
	v *= h;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);

	return glm::vec3(r*cu, v, -r*su);
}

glm::vec3 CMyApp::GetNorm(float u, float v)
{

	u *= 2 * 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, cv, su*sv);

}

glm::vec2 CMyApp::GetTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
}

bool CMyApp::Init()
{
	// törlési szín legyen fehér
	glClearColor(1,1,1, 1);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	//glDepthMask(GL_FALSE); // kikapcsolja a z-pufferbe történő írást - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glDepthMask.xml

	glLineWidth(4.0f); // a vonalprimitívek vastagsága: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLineWidth.xhtml
	//glEnable(GL_LINE_SMOOTH);

	// átlátszóság engedélyezése
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // meghatározza, hogy az átlátszó objektum az adott pixelben hogyan módosítsa a korábbi fragmentekből oda lerakott színt: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml

	// a raszterizált pontprimitívek mérete
	glPointSize(15.0f);

	//
	// shaderek betöltése
	//

	// így is létre lehetne hozni:

	// a shadereket tároló program létrehozása
	m_program.Init({ 
		{ GL_VERTEX_SHADER, "myVert.vert"},
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}
	},
	{ 
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	m_hengerProgram.Init({
	{ GL_VERTEX_SHADER, "myVert.vert"},
	{ GL_FRAGMENT_SHADER, "myFrag.frag"}
		},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	m_gombProgram.Init({
	{ GL_VERTEX_SHADER, "myVert.vert"},
	{ GL_FRAGMENT_SHADER, "myFrag.frag"}
		},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	m_program.LinkProgram();

	// tengelyeket kirajzoló shader rövid inicializálása
	m_axesProgram.Init({
		{GL_VERTEX_SHADER, "axes.vert"},
		{GL_FRAGMENT_SHADER, "axes.frag"}
	});
	// haladóknak, még rövidebb: 
	//m_axesProgram.Init({"axes.vert"_vs, "axes.frag"_fs});

	// kontrollpontokat kirajzoló program
	m_pointProgram.Init({
		{ GL_VERTEX_SHADER, "pointDrawer.vert" },
		{ GL_FRAGMENT_SHADER, "pointDrawer.frag" }
	});

	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe (m_buffer*) való feltöltése BufferData-val
	//

	// vertexek pozíciói:
	/*
	Az m_gpuBufferPos konstruktora már létrehozott egy GPU puffer azonosítót és a most következő BufferData hívás ezt 
		1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_gpuBufferPos típusa ArrayBuffer) és
		2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeit

	*/
	m_gpuBufferPos.BufferData( 
		std::vector<glm::vec3>{
			//		  X, Y, Z
			glm::vec3(-20,0, -20),
			glm::vec3(-20,0,  20), 
			glm::vec3( 20,0, -20),
			glm::vec3( 20,0,  20) 
		}
	);

	// normálisai
	m_gpuBufferNormal.BufferData(
		std::vector<glm::vec3>{
			// normal.X,.Y,.Z
			glm::vec3(0, 1, 0), // 0-ás csúcspont
			glm::vec3(0, 1, 0), // 1-es
			glm::vec3(0, 1, 0), // 2-es
			glm::vec3(0, 1, 0)  // 3-as
		}
	);

	// textúrakoordinátái
	m_gpuBufferTex.BufferData(
		std::vector<glm::vec2>{
			//        u, v
			glm::vec2(0, 0), // 0-ás csúcspont
			glm::vec2(1, 0), // 1-es
			glm::vec2(0, 1), // 2-es
			glm::vec2(1, 1)	 // 3-as
		}
	);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	m_gpuBufferIndices.BufferData(
		std::vector<int>{
			0,1,2,	// 1. háromszög: 0-1-2 csúcspontokból
			2,1,3	// 2. háromszög: 2-1-3 csúcspontokból
		}
	);

	// geometria VAO-ban való regisztrálása
	m_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);

	//henger---------------------------------------------------------

	std::vector<glm::vec3> hengerPos;
	std::vector<glm::vec3> hengerNorm;
	std::vector<glm::vec2> hengerText;

	hengerPos.resize((N + 1)*(M + 1));
	hengerNorm.resize((N + 1)*(M + 1));
	hengerText.resize((N + 1)*(M + 1));

	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			hengerPos[i + j * (N + 1)] = GetUV_henger(u, v);
			hengerNorm[i + j * (N + 1)] = GetNorm(u, -v);
			hengerText[i + j * (N + 1)] = GetTex(u, v);
		}

	std::vector<int> hengerInd;
	hengerInd.resize(3 * 2 * (N)*(M));

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			hengerInd[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			hengerInd[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			hengerInd[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			hengerInd[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			hengerInd[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			hengerInd[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}

	m_gpuBufferIndicesHenger.BufferData(hengerInd);
	m_gpuBufferPosHenger.BufferData(hengerPos);
	m_gpuBufferNormalHenger.BufferData(hengerNorm);
	m_gpuBufferTexHenger.BufferData(hengerText);

	m_vaoHenger.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPosHenger },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormalHenger },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTexHenger }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndicesHenger
	);

	//---------------------------------------------------------------

	//felso fedlap---------------------------------------------------

	std::vector<glm::vec3> felsolapPos;
	std::vector<glm::vec3> felsoNorm;
	std::vector<glm::vec2> felsoText;
	felsolapPos.resize(21);
	felsoNorm.resize(21);
	felsoText.resize(21);

	felsolapPos[0] = glm::vec3(0, 0, 0);
	felsoNorm[0] = glm::vec3(0, 1, 0);
	felsoText[0] = glm::vec2(0.5, 0.5);

	for (int i = 0; i < 20; ++i) {
		
		float str = (2 * M_PI / 20 * i);
		float x = cosf(str) * 0.5;
		float z = sinf(str) * 0.5;

		felsolapPos[i] = glm::vec3(x, 0, z);
		felsoNorm[i] = glm::vec3(0, 1, 0);
		felsoText[i] = glm::vec2(cosf(str) / 2.0f, sinf(str) / 2.0f);
	}

	std::vector<int> felsoInd;
	felsoInd.resize(3 * 20);

	int felsoC = 0;
	for (int i = 0; i < 20; ++i) {
		felsoInd[felsoC++] = i;
		felsoInd[felsoC++] = 21;
		felsoInd[felsoC++] = (i + 1) % 20;
	}

	m_gpuBufferIndicesFelsolap.BufferData(felsoInd);
	m_gpuBufferPosFelsolap.BufferData(felsolapPos);
	m_gpuBufferNormalFelsolap.BufferData(felsoNorm);
	m_gpuBufferTexFelsolap.BufferData(felsoText);

	m_vaoFelsolap.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPosFelsolap },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormalFelsolap },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTexFelsolap }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndicesFelsolap
	);

	//---------------------------------------------------------------

	// also fedlap---------------------------------------------------
	std::vector<glm::vec3> alsolapPos;
	std::vector<glm::vec3> alsoNorm;
	std::vector<glm::vec2> alsoText;
	alsolapPos.resize(21);
	alsoNorm.resize(21);
	alsoText.resize(21);

	alsolapPos[0] = glm::vec3(0, 0, 0);
	alsoNorm[0] = glm::vec3(0, 1, 0);
	alsoText[0] = glm::vec2(0.5, 0.5);

	for (int i = 0; i < 20; ++i) {

		float str = (2 * M_PI / 20 * i);
		float x = cosf(str) * 0.5;
		float z = sinf(str) * 0.5;

		alsolapPos[i] = glm::vec3(z, 0, x);
		alsoNorm[i] = glm::vec3(0, 1, 0);
		alsoText[i] = glm::vec2(cosf(str) / 2.0f, sinf(str) / 2.0f);
	}

	std::vector<int> alsoInd;
	alsoInd.resize(3 * 20);

	int alsoC = 0;
	for (int i = 0; i < 20; ++i) {
		alsoInd[alsoC++] = i;
		alsoInd[alsoC++] = 21;
		alsoInd[alsoC++] = (i + 1) % 20;
	}

	m_gpuBufferIndicesAlsolap.BufferData(alsoInd);
	m_gpuBufferPosAlsolap.BufferData(alsolapPos);
	m_gpuBufferNormalAlsolap.BufferData(alsoNorm);
	m_gpuBufferTexAlsolap.BufferData(alsoText);

	m_vaoAlsolap.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPosAlsolap },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormalAlsolap },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTexAlsolap }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndicesAlsolap
	);

	//---------------------------------------------------------------

	//felso lezaras egyik--------------------------------------------
	std::vector<glm::vec3> oldalegyPos;
	std::vector<glm::vec3> oldalegyNorm;
	std::vector<glm::vec2> oldalegyText;
	oldalegyPos.resize(21);
	oldalegyNorm.resize(21);
	oldalegyText.resize(21);

	oldalegyPos[0] = glm::vec3(0, 0, 0);
	oldalegyNorm[0] = glm::vec3(0, 1, 0);
	oldalegyText[0] = glm::vec2(0.5, 0.5);

	for (int i = 0; i < 20; ++i) {

		float str = (2 * M_PI / 20 * i);
		float x = cosf(str) * 0.5;
		float z = sinf(str) * 0.5;

		oldalegyPos[i] = glm::vec3(0, x, z);
		oldalegyNorm[i] = glm::vec3(0, 1, 0);
		oldalegyText[i] = glm::vec2(cosf(str) / 2.0f, sinf(str) / 2.0f);
	}

	std::vector<int> oldalegyInd;
	oldalegyInd.resize(3 * 20);

	int oldalegyC = 0;
	for (int i = 0; i < 20; ++i) {
		oldalegyInd[oldalegyC++] = i;
		oldalegyInd[oldalegyC++] = 21;
		oldalegyInd[oldalegyC++] = (i + 1) % 20;
	}

	m_gpuBufferIndicesOldalegy.BufferData(oldalegyInd);
	m_gpuBufferPosOldalegy.BufferData(oldalegyPos);
	m_gpuBufferNormalOldalegy.BufferData(oldalegyNorm);
	m_gpuBufferTexOldalegy.BufferData(oldalegyText);

	m_vaoOldalegy.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPosOldalegy },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormalOldalegy },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTexOldalegy }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndicesOldalegy
	);

	//---------------------------------------------------------------

	//felso lezaras masik--------------------------------------------
	std::vector<glm::vec3> oldalkettoPos;
	std::vector<glm::vec3> oldalkettoNorm;
	std::vector<glm::vec2> oldalkettoText;
	oldalkettoPos.resize(21);
	oldalkettoNorm.resize(21);
	oldalkettoText.resize(21);

	oldalkettoPos[0] = glm::vec3(0, 0, 0);
	oldalkettoNorm[0] = glm::vec3(0, 1, 0);
	oldalkettoText[0] = glm::vec2(0.5, 0.5);

	for (int i = 0; i < 20; ++i) {

		float str = (2 * M_PI / 20 * i);
		float x = cosf(str) * 0.5;
		float z = sinf(str) * 0.5;

		oldalkettoPos[i] = glm::vec3(0, z, x);
		oldalkettoNorm[i] = glm::vec3(0, 1, 0);
		oldalkettoText[i] = glm::vec2(cosf(str) / 2.0f, sinf(str) / 2.0f);
	}

	std::vector<int> oldalkettoInd;
	oldalkettoInd.resize(3 * 20);

	int oldalkettoC = 0;
	for (int i = 0; i < 20; ++i) {
		oldalkettoInd[oldalkettoC++] = i;
		oldalkettoInd[oldalkettoC++] = 21;
		oldalkettoInd[oldalkettoC++] = (i + 1) % 20;
	}

	m_gpuBufferIndicesOldalketto.BufferData(oldalkettoInd);
	m_gpuBufferPosOldalketto.BufferData(oldalkettoPos);
	m_gpuBufferNormalOldalketto.BufferData(oldalkettoNorm);
	m_gpuBufferTexOldalketto.BufferData(oldalkettoText);

	m_vaoOldalketto.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPosOldalketto },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormalOldalketto },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTexOldalketto }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndicesOldalketto
	);

	//---------------------------------------------------------------

	//gomb-----------------------------------------------------------

	std::vector<glm::vec3> gombPos;
	std::vector<glm::vec3> gombNorm;
	std::vector<glm::vec2> gombText;

	gombPos.resize((N + 1)*(M + 1));
	gombNorm.resize((N + 1)*(M + 1));
	gombText.resize((N + 1)*(M + 1));

	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			gombPos[i + j * (N + 1)] = GetPos(u, v);
			gombNorm[i + j * (N + 1)] = GetNorm(u, v);
			gombText[i + j * (N + 1)] = GetTex(u, v);
		}

	std::vector<int> gombInd;
	gombInd.resize(3 * 2 * (N)*(M));

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			gombInd[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			gombInd[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			gombInd[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			gombInd[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			gombInd[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			gombInd[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}

	m_gpuBufferIndicesGomb.BufferData(gombInd);
	m_gpuBufferPosGomb.BufferData(gombPos);
	m_gpuBufferNormalGomb.BufferData(gombNorm);
	m_gpuBufferTexGomb.BufferData(gombText);

	m_vaoGomb.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPosGomb },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormalGomb },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTexGomb }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndicesGomb
	);

	//---------------------------------------------------------------

	// textúra betöltése
	m_textureMetal.FromFile("texture.png");
	m_textureTest.FromFile("wall.png");

	// mesh betöltése
	m_mesh = ObjParser::parse("suzanne.obj");

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// tengelyek
	m_axesProgram.Use();
	m_axesProgram.SetUniform("mvp", m_camera.GetViewProj()*glm::translate(glm::vec3(0,0.1f,0)));

	glDrawArrays(GL_LINES, 0, 6);

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetal);

	// talaj
	glm::mat4 wallWorld = glm::translate(glm::vec3(0, 0, 0));
	m_program.SetUniform("world", wallWorld);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(wallWorld)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*wallWorld);
	m_program.SetUniform("Kd", glm::vec4(1,1,1, 1));

	m_vao.Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	m_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	for (int i = 0; i < 10; ++i) {
		// Egyik Oszlop ----------------------------------------------------------
		// henger
		m_hengerProgram.Use();
		glm::mat4 hengerWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10 ,0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) * //menetirányba fordítás
			glm::translate(glm::vec3(3, 0, 0)) * 
			glm::scale(glm::vec3(1, 7, 1));
		m_hengerProgram.SetUniform("world", hengerWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(hengerWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*hengerWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureMetal);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//felso fedlap
		glm::mat4 felsoWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) * 
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(3, 7, 0));
		m_program.SetUniform("world", felsoWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(felsoWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*felsoWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

		m_vaoFelsolap.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 20, GL_UNSIGNED_INT, 0);
		m_vaoFelsolap.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//also fedlap
		glm::mat4 alsoWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(3, 0, 0));
		m_program.SetUniform("world", alsoWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(alsoWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*alsoWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

		m_vaoAlsolap.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 20, GL_UNSIGNED_INT, 0);
		m_vaoAlsolap.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//-----------------------------------------------------------------------------------------------------

		// Masik Oszlop ----------------------------------------------------------
		// henger
		m_hengerProgram.Use();
		glm::mat4 hengermasikWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(-3, 0, 0)) * glm::scale(glm::vec3(1, 7, 1));
		m_hengerProgram.SetUniform("world", hengermasikWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(hengermasikWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*hengermasikWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureMetal);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//felso fedlap
		glm::mat4 felsomasikWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) * 
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(-3, 7, 0));
		m_program.SetUniform("world", felsomasikWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(felsomasikWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*felsomasikWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

		m_vaoFelsolap.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 20, GL_UNSIGNED_INT, 0);
		m_vaoFelsolap.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//also fedlap
		glm::mat4 alsomasikWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) * 
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(-3, 0, 0));
		m_program.SetUniform("world", alsomasikWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(alsomasikWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*alsomasikWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

		m_vaoAlsolap.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 20, GL_UNSIGNED_INT, 0);
		m_vaoAlsolap.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//-----------------------------------------------------------------------------------------------------

		//henger lentebbi
		m_hengerProgram.Use();
		glm::mat4 lentebbikWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(3, 5, 0)) * glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(1, 6, 1));
		m_hengerProgram.SetUniform("world", lentebbikWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(lentebbikWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*lentebbikWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureMetal);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//henger legfenti
		m_hengerProgram.Use();
		glm::mat4 fentikWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(3.5, 7, 0)) * glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(1, 7, 1));
		m_hengerProgram.SetUniform("world", fentikWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(fentikWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*fentikWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureMetal);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		glm::mat4 oldalegyWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(-3.5, 7, 0));
		m_program.SetUniform("world", oldalegyWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(oldalegyWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*oldalegyWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

		m_vaoOldalegy.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 20, GL_UNSIGNED_INT, 0);
		m_vaoOldalegy.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		glm::mat4 oldalkettoWorld = 
			glm::translate(glm::vec3(cos(2 * M_PI / 10 * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / 10) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(3.5, 7, 0));
		m_program.SetUniform("world", oldalkettoWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(oldalkettoWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*oldalkettoWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

		m_vaoOldalketto.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 20, GL_UNSIGNED_INT, 0);
		m_vaoOldalketto.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	}

	for (int i = 0; i < num; ++i) {
		// Fej------------------------------------------------------------------------
		m_gombProgram.Use();
		glm::mat4 gombWorld = 
			glm::rotate<float>(SDL_GetTicks() / 1000.0f * (2*M_PI) / 5 ,glm::vec3(0,1,0)) *
			glm::translate(glm::vec3(cos(2 * M_PI / num * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / num) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(0, 3.5, 0));
		m_gombProgram.SetUniform("world", gombWorld);
		m_gombProgram.SetUniform("worldIT", glm::transpose(glm::inverse(gombWorld)));
		m_gombProgram.SetUniform("MVP", m_camera.GetViewProj()*gombWorld);
		m_gombProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_gombProgram.SetTexture("texImage", 0, m_textureMetal);

		m_vaoGomb.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoGomb.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//----------------------------------------------------------------------------------

		// Test ----------------------------------------------------------
		// henger
		m_hengerProgram.Use();
		glm::mat4 TestWorld = 
			glm::rotate<float>(SDL_GetTicks() / 1000.0f * (2 * M_PI) / 5, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(cos(2 * M_PI / num * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / num) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(1, 2, 1));
		m_hengerProgram.SetUniform("world", TestWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(TestWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*TestWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureTest);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//felso fedlap
		glm::mat4 fentTestWorld = 
			glm::rotate<float>(SDL_GetTicks() / 1000.0f * (2 * M_PI) / 5, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(cos(2 * M_PI / num * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / num) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(0, 2, 0));
		m_program.SetUniform("world", fentTestWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(fentTestWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*fentTestWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

		m_vaoFelsolap.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 20, GL_UNSIGNED_INT, 0);
		m_vaoFelsolap.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//also fedlap
		glm::mat4 lentTestWorld =
			glm::rotate<float>(SDL_GetTicks() / 1000.0f * (2 * M_PI) / 5, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(cos(2 * M_PI / num * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / num) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(0, 1, 0));
		m_program.SetUniform("world", lentTestWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(lentTestWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*lentTestWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

		m_vaoAlsolap.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 20, GL_UNSIGNED_INT, 0);
		m_vaoAlsolap.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

		//-----------------------------------------------------------------------------------------------------

		//kez -jobb
		m_hengerProgram.Use();
		glm::mat4 kezWorld =
			glm::rotate<float>(SDL_GetTicks() / 1000.0f * (2 * M_PI) / 5, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(cos(2 * M_PI / num * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / num) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(1.3, 1.7, 0)) *
			glm::rotate<float>(-M_PI / 4, glm::vec3(0, 0, 1)) *
			glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1)) *
			glm::scale(glm::vec3(1, 2, 1));;
		m_hengerProgram.SetUniform("world", kezWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(kezWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*kezWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureTest);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját*/

		//kez -bal
		m_hengerProgram.Use();
		glm::mat4 kezkettoWorld =
			glm::rotate<float>(SDL_GetTicks() / 1000.0f * (2 * M_PI) / 5, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(cos(2 * M_PI / num * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / num) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(-1.3, 1.7, 0)) *
			glm::rotate<float>(M_PI / 4, glm::vec3(0, 0, 1)) *
			glm::rotate<float>(-M_PI / 2, glm::vec3(0, 0, 1)) *
			glm::scale(glm::vec3(1, 2, 1));
		m_hengerProgram.SetUniform("world", kezkettoWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(kezkettoWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*kezkettoWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureTest);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját*/

		//lab -jobb
		m_hengerProgram.Use();
		glm::mat4 labWorld =
			glm::rotate<float>(SDL_GetTicks() / 1000.0f * (2 * M_PI) / 5, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(cos(2 * M_PI / num * i) * 10,0,sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / num) * -i + M_PI,glm::vec3(0,1,0)) *
			glm::translate(glm::vec3(1.3, 0.3, 0)) *
			glm::rotate<float>(-M_PI / 4, glm::vec3(0, 0, 1)) *
			glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1)) *
			glm::scale(glm::vec3(1, 2, 1));;
		m_hengerProgram.SetUniform("world", labWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(labWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*labWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureTest);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját*/

		//lab -bal
		m_hengerProgram.Use();
		glm::mat4 labkettoWorld =
			glm::rotate<float>(SDL_GetTicks() / 1000.0f * (2 * M_PI) / 5, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(cos(2 * M_PI / num * i) * 10, 0, sin(2 * M_PI / 10 * i) * 10)) *
			glm::rotate<float>((2 * M_PI / num) * -i + M_PI, glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(-1.3, 0.3, 0)) *
			glm::rotate<float>(M_PI / 4, glm::vec3(0, 0, 1)) *
			glm::rotate<float>(-M_PI / 2, glm::vec3(0, 0, 1)) *
			glm::scale(glm::vec3(1, 2, 1));
		m_hengerProgram.SetUniform("world", labkettoWorld);
		m_hengerProgram.SetUniform("worldIT", glm::transpose(glm::inverse(labkettoWorld)));
		m_hengerProgram.SetUniform("MVP", m_camera.GetViewProj()*labkettoWorld);
		m_hengerProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		m_hengerProgram.SetTexture("texImage", 0, m_textureTest);

		m_vaoHenger.Bind();
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, 0);
		m_vaoHenger.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját*/
	}

	

	// végetért a 3D színtér rajzolása
	m_gombProgram.Unuse();
	m_hengerProgram.Unuse();
	m_program.Unuse();

	//
	// UI
	//
	// A következő parancs megnyit egy ImGui tesztablakot és így látszik mit tud az ImGui.
	ImGui::ShowTestWindow();
	// A ImGui::ShowTestWindow implementációja az imgui_demo.cpp-ben található
	// Érdemes még az imgui.h-t böngészni, valamint az imgui.cpp elején a FAQ-ot elolvasni.
	// Rendes dokumentáció nincs, de a fentiek elegendőek kell legyenek.
	
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiSetCond_FirstUseEver);
	// csak akkor lépjünk be, hogy ha az ablak nincs csíkká lekicsinyítve...
	if (ImGui::Begin("ZH"))
	{

		if (ImGui::Button("gomb")) {
			std::cout << "hello" << std::endl;
		}

		ImGui::InputInt("integer-emberszam", &num);
		/*ImGui::SliderFloat("alpha", &alpha, -1, 2);
		ImGui::SliderFloat3("color", &(color[0]), 0, 1);
		ImGui::SliderFloat("energy_remaining", &m_energyReamining, 0, 2);

		if (ImGui::Button("Randomize positions!")) {
			randomPositions();
		}

		if (ImGui::Button("Randomize Velocities!")) {
			randomVelocities();
		}

		ImGui::SliderInt("Particle num", &m_particleCount, 1, 100);*/
	}
	ImGui::End(); // ...de még ha le is volt, End()-et hívnunk kell
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
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
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}

glm::vec3 CMyApp::Eval(float t)
{
	if (m_controlPoints.size() == 0)
		return glm::vec3(0);

	int interval = (int)t;

	if (interval < 0)
		return m_controlPoints[0];
		
	if (interval >= m_controlPoints.size()-1)
		return m_controlPoints[m_controlPoints.size()-1];

	float localT = t - interval;
	return (1- localT)*m_controlPoints[interval] + localT*m_controlPoints[interval+1];
}
