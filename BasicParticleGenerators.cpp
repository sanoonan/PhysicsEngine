#include "BasicParticleGenerators.h"
#include <glm/gtc/constants.hpp>


using namespace std;


void BoxPosGen :: generate(double dt, ParticleData *p, size_t startId, size_t endId)
{
	glm::vec4 posMin
	(
		m_pos.x - m_maxStartPosOffset.x,
		m_pos.y - m_maxStartPosOffset.y,
		m_pos.z - m_maxStartPosOffset.z,
		1.0
	);

	glm::vec4 posMax
	(
		m_pos.x + m_maxStartPosOffset.x,
		m_pos.y + m_maxStartPosOffset.y,
		m_pos.z + m_maxStartPosOffset.z,
		1.0
	);

	for (size_t i=startId; i<endId; ++i)
		p->m_pos[i] = glm::linearRand(posMin, posMax);

}


void RoundPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
{
	float m_pi = glm::pi<float>();

	for (size_t i = startId; i < endId; ++i)
	{
		double ang = glm::linearRand(0.0, m_pi*2.0);
		p->m_pos[i] = m_centre + glm::vec4(m_radX*sin(ang), m_radY*cos(ang), 0.0, 1.0);
	}
}

void BasicColorGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
{
	for (size_t i = startId; i < endId; ++i)
	{
		glm::vec4 s_col = glm::linearRand(m_minStartCol, m_maxStartCol);
		p->m_startCol[i] = s_col;
		p->m_endCol[i] = glm::linearRand(m_minEndCol, m_maxEndCol);
	}
}

void BasicVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
{
	for (size_t i = startId; i < endId; ++i)
	{
		p->m_vel[i] = glm::linearRand(m_minStartVel, m_maxStartVel);
	}
}


void BasicTimeGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
{
	for (size_t i = startId; i < endId; ++i)
	{
		p->m_time[i].x = p->m_time[i].y = glm::linearRand(m_minTime, m_maxTime);
		p->m_time[i].z = (float)0.0;
		p->m_time[i].w = (float)1.0 / p->m_time[i].x;
	}
}

void MySpinningGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
{
	glm::vec4 new_pos((m_centre.x + m_rad*cos(m_ang)), m_centre.y, (m_centre.z + m_rad*sin(m_ang)), 1.0);

	for (size_t i = startId; i < endId; ++i)
		p->m_pos[i] = new_pos;

	m_ang += m_speed;
}

