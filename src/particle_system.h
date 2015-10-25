#ifndef INCLUDED_PARTICLE_SYSTEM_H
#define INCLUDED_PARTICLE_SYSTEM_H

#include "lib/gfx/rgb_colour.h"
#include "lib/containers/pool_allocator.h"
#include "lib/vector3.h"


// ****************************************************************************
// Particle
// ****************************************************************************

class Particle
{
public:
	Vector3         m_pos;
	Vector3         m_vel;
	float           m_birthTime;
    RgbaColour      m_colour;
	
    void Initialise(Vector3 const &pos, Vector3 const &vel);
	bool Advance();
	void Render(float predictionTime);
};



// ****************************************************************************
// ParticleSystem
// ****************************************************************************

class ParticleSystem
{
private:
	PoolAllocator <Particle> m_particles;

public:
	void CreateParticle(Vector3 const &_pos, Vector3 const &_vel);

	void Advance();
	void Render();
	void Empty();
};


extern ParticleSystem g_particleSystem;


#endif
