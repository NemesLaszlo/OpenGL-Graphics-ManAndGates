#pragma once

// C++ includes
#include <memory>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "ProgramObject.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"

#include "Mesh_OGL3.h"
#include "gCamera.h"

#include <vector>

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	glm::vec3 GetPos(float u, float v);

	glm::vec3 GetUV_henger(float u, float v);

	glm::vec3 GetNorm(float u, float v);

	glm::vec2 GetTex(float u, float v);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:
	// g�rb�nk ki�rt�kel�se
	glm::vec3 Eval(float);
	float	m_currentParam{ 0 };

	// shaderekhez sz�ks�ges v�ltoz�k
	ProgramObject		m_program;		// shaderek programja
	ProgramObject		m_axesProgram;
	ProgramObject		m_pointProgram;
	ProgramObject		m_hengerProgram;
	ProgramObject		m_gombProgram;

	Texture2D			m_textureMetal;
	Texture2D			m_textureTest;

	VertexArrayObject	m_vao;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTex;	// text�rakoordin�t�k t�mbje

	VertexArrayObject	m_vaoHenger;			// VAO objektum
	IndexBuffer			m_gpuBufferIndicesHenger;		// indexek
	ArrayBuffer			m_gpuBufferPosHenger;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormalHenger;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTexHenger;	// text�rakoordin�t�k t�mbje

	VertexArrayObject	m_vaoFelsolap;			// VAO objektum
	IndexBuffer			m_gpuBufferIndicesFelsolap;		// indexek
	ArrayBuffer			m_gpuBufferPosFelsolap;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormalFelsolap;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTexFelsolap;	// text�rakoordin�t�k t�mbje

	VertexArrayObject	m_vaoAlsolap;			// VAO objektum
	IndexBuffer			m_gpuBufferIndicesAlsolap;		// indexek
	ArrayBuffer			m_gpuBufferPosAlsolap;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormalAlsolap;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTexAlsolap;	// text�rakoordin�t�k t�mbje

	VertexArrayObject	m_vaoOldalegy;			// VAO objektum
	IndexBuffer			m_gpuBufferIndicesOldalegy;		// indexek
	ArrayBuffer			m_gpuBufferPosOldalegy;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormalOldalegy;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTexOldalegy;	// text�rakoordin�t�k t�mbje

	VertexArrayObject	m_vaoOldalketto;			// VAO objektum
	IndexBuffer			m_gpuBufferIndicesOldalketto;		// indexek
	ArrayBuffer			m_gpuBufferPosOldalketto;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormalOldalketto;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTexOldalketto;	// text�rakoordin�t�k t�mbje

	VertexArrayObject	m_vaoGomb;			// VAO objektum
	IndexBuffer			m_gpuBufferIndicesGomb;		// indexek
	ArrayBuffer			m_gpuBufferPosGomb;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormalGomb;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTexGomb;	// text�rakoordin�t�k t�mbje

	std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	const int kMaxPointCount = 10;

	static const int N = 20;
	static const int M = 20;
	int num = 10;


	std::vector<glm::vec3>		m_controlPoints{ {-10,0,-10}, {10,0,-10} };
	std::vector<std::string>	m_pointName{ "Point 1", "Point 2" };
};

