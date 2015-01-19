#ifndef __BASICPARTICLEUPDATERS_H__
#define __BASICPARTICLEUPDATERS_H__

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

#include "BasicParticles.h"

using namespace std;

class EulerUpdater : public ParticleUpdater
{
public:
	glm::vec4 m_globalAcceleration;

	EulerUpdater() { m_globalAcceleration = glm::vec4(0.0);}
	virtual void update(double dt, ParticleData *p) override;

};

class FloorUpdater : public ParticleUpdater
{
public:
	float m_floorY;
	float m_bounceFactor;

	FloorUpdater() { m_floorY=0.0f; m_bounceFactor = 0.0f; }
	virtual void update(double dt, ParticleData *p) override;
};



class BasicColorUpdater : public ParticleUpdater
{
public:
	virtual void update(double dt, ParticleData *p) override;
};


class BasicTimeUpdater : public ParticleUpdater
{
public:
	virtual void update(double dt, ParticleData *p) override;
};


class MyDragUpdater : public ParticleUpdater
{
public:
	float m_dragCoef;

	MyDragUpdater() { m_dragCoef = 0.0f; }
	MyDragUpdater(float dragCoef) { m_dragCoef = dragCoef; }
	virtual void update(double dt, ParticleData *p) override;
};


class MyPlaneUpdater : public ParticleUpdater
{
public:
	glm::vec4 m_topLeft;
	glm::vec4 m_bottomRight;
	glm::vec4 m_normal;
	float m_bounceFactor;

	MyPlaneUpdater() { m_topLeft=glm::vec4(0.0f); m_bottomRight = glm::vec4(0.0f); m_bounceFactor = 0.0f; m_normal = glm::vec4(0.0f); }
	virtual void update(double dt, ParticleData *p) override;


};

class MyColourVelUpdater : public ParticleUpdater
{
public:
	float m_velLimit;

	MyColourVelUpdater() { m_velLimit = 0.0f; }
	virtual void update(double dt, ParticleData *p) override;
};


void MyForwardEuler(glm::vec4 &x, glm::vec4 &v, glm::vec4 a, float dt);
void MyRK4(glm::vec4 &x, glm::vec4 &v, glm::vec4 a, float dt);

class MyRK4Updater : public ParticleUpdater
{
public:
	glm::vec4 m_constantAcceleration;

	MyRK4Updater() { m_constantAcceleration = glm::vec4(0.0);}
	virtual void update(double dt, ParticleData *p) override;

};

class VariedAccelUpdater : public ParticleUpdater
{
public:
	VariedAccelUpdater() {}
	virtual void update(double dt, ParticleData *p) override;
};

glm::vec4 accFunc(float dt);

void VaryRK4(glm::vec4 &x, glm::vec4 &v, glm::vec4 &a, float dt);


#endif __BASICPARTICLEUPDATERS_H__