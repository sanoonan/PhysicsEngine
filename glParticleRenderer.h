#pragma once

#define _CRT_SECURE_NO_DEPRECATE
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>


// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <string>
#include <memory>
#include <vector>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "particleRenderer.h"

using namespace std;

class GLParticleRenderer : public IParticleRenderer
{
protected:
	ParticleSystem *m_system;

	unsigned int m_bufPos;
	unsigned int m_bufCol;
	unsigned int m_vao;


public:

	GLParticleRenderer() { m_system = nullptr; m_bufPos = 0; m_bufCol= 0; m_vao = 0;}
	~GLParticleRenderer() { destroy(); }

	void generate(ParticleSystem *sys, bool useQuads) override;
	void destroy() override;
	void update() override;
	void render() override;
};