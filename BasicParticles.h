#ifndef __BASICPARTICLES_H__
#define __BASICPARTICLES_H__

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

using namespace std;

class ParticleData
{
public:



	float m_mass;
	std::unique_ptr<glm::vec4[]> m_pos;
	std::unique_ptr<glm::vec4[]> m_col;
	std::unique_ptr<glm::vec4[]> m_startCol;
	std::unique_ptr<glm::vec4[]> m_endCol;
	std::unique_ptr<glm::vec4[]> m_vel;
	std::unique_ptr<glm::vec4[]> m_acc;
	std::unique_ptr<glm::vec4[]> m_time;

	std::unique_ptr<bool[]> m_alive;

	size_t m_count;
	size_t m_countAlive;

	ParticleData(){}
	explicit ParticleData(size_t maxCount) { generate(maxCount); }
	~ParticleData() { }

//	ParticleData(const ParticleData &) = delete;
//	ParticleData &operator=(const ParticleData &) = delete;

	void generate(size_t maxSize);
	void kill(size_t id);
	void wake(size_t id);
	void swapData(size_t a, size_t b);



	struct PosCol
	{
		glm::vec4 pos;
		glm::vec4 col;
		float cam_dist;

		bool operator<(const PosCol& that)
		{
			return this->cam_dist > that.cam_dist;
		}
/*
		float operator-(const PosCol& that)
		{
			return that.cam_dist - this->cam_dist;
		}*/
	};

	std::unique_ptr<PosCol[]> buffer_array;
	void sortBufferArray(glm::vec3 cam_pos);
	std::unique_ptr<glm::vec4[]> sorted_pos;
	std::unique_ptr<glm::vec4[]> sorted_col;



	
};

//bool compare_PosCol(const ParticleData::PosCol& a, const ParticleData::PosCol& b);

class ParticleGenerator
{
public:
	ParticleGenerator() { }
	virtual ~ParticleGenerator() { }

	virtual void generate(double dt, ParticleData *p, size_t startID, size_t endId) = 0;
};




class ParticleEmitter
{
protected:
	std::vector<std::shared_ptr<ParticleGenerator>> m_generators;

public:
	float m_emitRate;

	
	ParticleEmitter() { m_emitRate = 0.0; }
	virtual ~ParticleEmitter() { }

	//calls all the generators and at the end it activates (wakes) particles
	virtual void emit(double dt, ParticleData *p);

	void addGenerator(std::shared_ptr<ParticleGenerator> gen) { m_generators.push_back(gen); }
};



class ParticleUpdater
{
public:
	ParticleUpdater() { }
	virtual ~ParticleUpdater() { }

	virtual void update (double dt, ParticleData *p) = 0;
};



class ParticleSystem
{

protected:
	ParticleData m_particles;
	ParticleData m_aliveParticles;

	size_t m_count;

	std::vector<std::shared_ptr<ParticleEmitter>> m_emitters;
	std::vector<std::shared_ptr<ParticleUpdater>> m_updaters;

public:

	explicit ParticleSystem(size_t maxCount);
	virtual  ~ParticleSystem() { }

	
//	ParticleSystem(const ParticleSystem &) = delete;
//	ParticleSystem &operator=(const ParticleSystem &) = delete;

	virtual void update(double dt);
	virtual void reset();
	

	virtual size_t numAllParticles() const { return m_particles.m_count; }
	virtual size_t numAliveParticles() const { return m_particles.m_countAlive; }

	void addEmitter (std::shared_ptr<ParticleEmitter> em) { m_emitters.push_back(em); }
	void addUpdater(std::shared_ptr<ParticleUpdater> up) { m_updaters.push_back(up); }

	ParticleData *finalData() { return &m_particles; }

	void sortParticles(glm::vec3 cam_pos);
};



#endif __BASICPARTICLES_H__