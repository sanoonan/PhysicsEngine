#ifndef __BASICPARTICLEGENERATORS_H__
#define __BASICPARTICLEGENERATORS_H__

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
#include <glm/gtc/random.hpp>

#include "BasicParticles.h"

using namespace std;

class BoxPosGen : public ParticleGenerator
{
public:
	glm::vec4 m_pos;
	glm::vec4 m_maxStartPosOffset;


	BoxPosGen() { m_pos = glm::vec4(0.0); m_maxStartPosOffset = glm::vec4(0.0);}

	virtual void generate (double dt, ParticleData *p, size_t startId, size_t endId) override;
};


class RoundPosGen : public ParticleGenerator
{
public:
	glm::vec4 m_centre;
	float m_radX;
	float m_radY;


	RoundPosGen() { m_centre = glm::vec4(0.0); m_radX = 0.0f; m_radY = 0.0f; }
	RoundPosGen(const glm::vec4 &centre, double radX, double radY) {m_centre = centre; m_radX = (float)radX; m_radY = (float)radY; }

	virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
};

class BasicColorGen : public ParticleGenerator
{
public:
	glm::vec4 m_minStartCol;
	glm::vec4 m_maxStartCol;
	glm::vec4 m_minEndCol;
	glm::vec4 m_maxEndCol;

	BasicColorGen() { m_minStartCol = glm::vec4(0.0); m_maxStartCol = glm::vec4(0.0); m_minEndCol = glm::vec4(0.0); m_maxEndCol = glm::vec4(0.0); }
 
	virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
};



class BasicVelGen : public ParticleGenerator
{
public:
	glm::vec4 m_minStartVel;
	glm::vec4 m_maxStartVel;
public:
	BasicVelGen() { m_minStartVel = glm::vec4(0.0); m_maxStartVel = glm::vec4(0.0); }
 
	virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
};


class BasicTimeGen : public ParticleGenerator
{
public:
	float m_minTime;
	float m_maxTime;
public:
	BasicTimeGen() { m_minTime = 0.0f; m_maxTime = 0.0f;}
 
	virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
};

class MySpinningGen : public ParticleGenerator
{
public:

	glm::vec4 m_centre;
	float m_rad;
	float m_speed;
	float m_ang;

	MySpinningGen() { m_centre = glm::vec4(0.0f); m_rad = 0.0f; m_speed = 0; m_ang = 0.0f; }
	MySpinningGen(glm::vec4 centre, float rad, float speed) { m_centre = centre; m_rad = rad; m_speed = speed;  m_ang = 0.0f; }

	virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;

};



#endif __BASICPARTICLEGENERATORS_H__