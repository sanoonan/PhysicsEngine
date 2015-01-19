#include "BasicParticleUpdaters.h"

using namespace std;

void EulerUpdater::update(double dt, ParticleData *p)
{
	const glm::vec4 globalA
		(
			dt * m_globalAcceleration.x,
			dt * m_globalAcceleration.y,
			dt * m_globalAcceleration.z,
			0.0
		);

	const float localDT = (float)dt;

	const unsigned int endId = p->m_countAlive;

	for(size_t i=0; i<endId; ++i)
		p->m_acc[i] += globalA;
/*
	for(size_t i=0; i<endId; ++i)
		p->m_vel[i] += localDT*p->m_acc[i];

	for(size_t i=0; i<endId; ++i)
		p->m_pos[i] += localDT*p->m_vel[i];
	*/	

	for(size_t i=0; i<endId; ++i)
		MyForwardEuler(p->m_pos[i], p->m_vel[i], p->m_acc[i], localDT);
}




void FloorUpdater::update(double dt, ParticleData *p)
{
	const float localDT = (float)dt;
 
	const size_t endId = p->m_countAlive;
	for (size_t i = 0; i < endId; ++i)
	{
		if (p->m_pos[i].y < m_floorY)
		{
			glm::vec4 force = p->m_acc[i];
			float normalFactor = glm::dot(force, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			if (normalFactor < 0.0f)
				force -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * normalFactor;
 
			float velFactor = glm::dot(p->m_vel[i], glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			//if (velFactor < 0.0)
			p->m_vel[i] -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_bounceFactor) * velFactor;
 
			p->m_acc[i] = force;
		}
	}
 
}

void BasicColorUpdater::update(double dt, ParticleData *p)
{
	const size_t endId = p->m_countAlive;
	for (size_t i = 0; i < endId; ++i)
		p->m_col[i] = glm::mix(p->m_startCol[i], p->m_endCol[i], p->m_time[i].z);

}

void BasicTimeUpdater::update(double dt, ParticleData *p)
{
	unsigned int endId = p->m_countAlive;
	const float localDT = (float)dt;
 
	if (endId == 0) return;
 
	for (size_t i = 0; i < endId; ++i)
	{
		p->m_time[i].x -= localDT;
		// interpolation: from 0 (start of life) till 1 (end of life)
		p->m_time[i].z = (float)1.0 - (p->m_time[i].x*p->m_time[i].w); // .w is 1.0/max life time		
 
		if (p->m_time[i].x < (float)0.0)
		{
			p->kill(i);
			endId = p->m_countAlive < p->m_count ? p->m_countAlive : p->m_count;
		}
	}
}

void MyDragUpdater::update(double dt, ParticleData *p)
{
	unsigned int endId = p->m_countAlive;
	const float localDT = (float)dt;

	for(size_t i=0; i<endId; ++i)
	{
		glm::vec4 globalA
		(
		(-m_dragCoef*p->m_vel[i].x)/p->m_mass, 
		(-m_dragCoef*p->m_vel[i].y)/p->m_mass, 
		(-m_dragCoef*p->m_vel[i].z)/p->m_mass, 
		0.0f
		);

		p->m_acc[i] += globalA;
	}

	for(size_t i=0; i<endId; ++i)
		p->m_vel[i] += localDT * p->m_acc[i];

	for(size_t i=0; i<endId; ++i)
		p->m_pos[i] += localDT * p->m_vel[i];
}



void MyPlaneUpdater::update(double dt, ParticleData *p)
{
	

	const float localDT = (float)dt;
	float distx = glm::abs(m_topLeft.x - m_bottomRight.x);
	float disty = glm::abs(m_topLeft.y - m_bottomRight.y);
	float distz = glm::abs(m_topLeft.z - m_bottomRight.z);
 
	const size_t endId = p->m_countAlive;
	for (size_t i = 0; i < endId; ++i)
	{
		if (p->m_pos[i].x < -0.501f)
			int q = 0;

		bool collision = true;
		if(distx == 0)
		{
			float disty1 = glm::abs(m_topLeft.y - p->m_pos[i].y);
			float disty2 = glm::abs(m_bottomRight.y - p->m_pos[i].y);

			float distz1 = glm::abs(m_topLeft.z - p->m_pos[i].z);
			float distz2 = glm::abs(m_bottomRight.z - p->m_pos[i].z);

			if((disty1>disty)||(disty2 > disty))
				collision = false;

			if((distz1>distz)||(distz2 > distz))
				collision = false;

			if(collision)
			{
				if(m_normal.x > 0)
					if(p->m_pos[i].x > m_topLeft.x)
						collision = false;

				if(m_normal.x < 0)
					if(p->m_pos[i].x < m_topLeft.x)
						collision = false;
			}
		}
		else if(disty == 0)
		{
			float distx1 = glm::abs(m_topLeft.x - p->m_pos[i].x);
			float distx2 = glm::abs(m_bottomRight.x - p->m_pos[i].x);

			float distz1 = glm::abs(m_topLeft.z - p->m_pos[i].z);
			float distz2 = glm::abs(m_bottomRight.z - p->m_pos[i].z);

			if((distx1>distx)||(distx2 > distx))
				collision = false;

			if((distz1>distz)||(distz2 > distz))
				collision = false;

			if(collision)
			{
				if(m_normal.y > 0)
					if(p->m_pos[i].y > m_topLeft.y)
						collision = false;

				if(m_normal.y < 0)
					if(p->m_pos[i].y < m_topLeft.y)
						collision = false;
			}
		}
		else if(distz == 0)
		{
			float distx1 = glm::abs(m_topLeft.x - p->m_pos[i].x);
			float distx2 = glm::abs(m_bottomRight.x - p->m_pos[i].x);

			float disty1 = glm::abs(m_topLeft.y - p->m_pos[i].y);
			float disty2 = glm::abs(m_bottomRight.y - p->m_pos[i].y);

			if((distx1>distx)||(distx2 > distx))
				collision = false;

			if((disty1>disty)||(disty2 > disty))
				collision = false;

			if(collision)
			{
				if(m_normal.z > 0)
					if(p->m_pos[i].z > m_topLeft.z)
						collision = false;

				if(m_normal.z < 0)
					if(p->m_pos[i].z < m_topLeft.z)
						collision = false;
			}
		}

		if (collision)
		{
			glm::vec4 force = p->m_acc[i];
			float normalFactor = glm::dot(force, m_normal);
			if (normalFactor < 0.0f)
				force -= m_normal * normalFactor;
 
			float velFactor = glm::dot(p->m_vel[i], m_normal);
			//if (velFactor < 0.0)
			p->m_vel[i] -= m_normal * (1.0f + m_bounceFactor) * velFactor;
 
			p->m_acc[i] = force;
		}
	}
 
}

void MyColourVelUpdater::update(double dt, ParticleData *p)
{
	const size_t endId = p->m_countAlive;

	for (size_t i = 0; i < endId; ++i)
	{
		if(m_velLimit == 0.0f)
			p->m_col[i] = p->m_startCol[i];
		else
		{
			float vel = glm::length(p->m_vel[i]);
			p->m_col[i] = glm::mix(p->m_startCol[i], p->m_endCol[i], vel/m_velLimit);
		}
	}
}

void MyForwardEuler(glm::vec4 &x, glm::vec4 &v, glm::vec4 a, float dt)
{
	//assumes acceleration is constant
	v += dt * a;
	x += dt * v;
}



void MyRK4(glm::vec4 &x, glm::vec4 &v, glm::vec4 a, float dt)
{
	//assumes acceleration is constant
	glm::vec4 x1, x2, x3, x4, v1, v2, v3, v4;

	x1 = x;
	v1 = v;

	x2 = x + 0.5f*v1*dt;
	v2 = v + 0.5f*a*dt;

	x3 = x + 0.5f*v2*dt;
	v3 = v + 0.5f*a*dt;

	x4 = x + v3*dt;
	v4 = v + a*dt;

	x += (dt/6)*(v1 + 2.0f*v2 + 2.0f*v3 + v4);
	v += (dt/6)*(a + 2.0f*a + 2.0f*a + a);
}



void MyRK4Updater::update(double dt, ParticleData *p)
{
	const glm::vec4 a
		(
			dt * m_constantAcceleration.x,
			dt * m_constantAcceleration.y,
			dt * m_constantAcceleration.z,
			0.0
		);

	const float localDT = (float)dt;

	const unsigned int endId = p->m_countAlive;

	for(size_t i=0; i<endId; ++i)
		p->m_acc[i] += a;

	for(size_t i=0; i<endId; ++i)
		MyRK4(p->m_pos[i], p->m_vel[i], p->m_acc[i], localDT);
}

void VariedAccelUpdater::update(double dt, ParticleData *p)
{
	const float localDT = (float)dt;

	const unsigned int endId = p->m_countAlive;

	for(size_t i=0; i<endId; ++i)
		VaryRK4(p->m_pos[i], p->m_vel[i], p->m_acc[i], localDT);
}

void VaryRK4(glm::vec4 &x, glm::vec4 &v, glm::vec4 &a, float dt)
{
	//assumes acceleration is constant
	glm::vec4 x1, x2, x3, x4, v1, v2, v3, v4, a1, a2, a3, a4;

	x1 = x;
	v1 = v;
	a1 = accFunc(0);

	x2 = x + 0.5f*v1*dt;
	v2 = v + 0.5f*a1*dt;
	a2 = accFunc(dt/2); 

	x3 = x + 0.5f*v2*dt;
	v3 = v + 0.5f*a2*dt;
	a3 = accFunc(dt/2);

	x4 = x + v3*dt;
	v4 = v + a3*dt;
	a4 = accFunc(dt);

	x += (dt/6)*(v1 + 2.0f*v2 + 2.0f*v3 + v4);
	v += (dt/6)*(a1 + 2.0f*a2 + 2.0f*a3 + a4);
	a = accFunc(dt);
}

glm::vec4 accFunc(float dt)
{ 
	return glm::vec4(dt, 0, 0, 0.0f);
}