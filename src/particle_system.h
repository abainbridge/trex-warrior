#ifndef INCLUDED_PARTICLE_SYSTEM_H
#define INCLUDED_PARTICLE_SYSTEM_H

#include "lib/gfx/rgb_colour.h"
#include "lib/containers/pool_allocator.h"
#include "lib/vector3.h"


// ****************************************************************************
// ParticleType
// ****************************************************************************

class ParticleType
{
public:
	float m_fadeInTime;
	float m_fadeOutTime;
	float m_life;
	float m_size;
	float m_friction;		// Amount of friction to apply (0=none 1=a huge amount)
	float m_gravity;		// Amount of gravity to apply (0=none 1=quite a lot)
};


// ****************************************************************************
// Particle
// ****************************************************************************

class Particle
{
public:
	Vector3         m_pos;
	Vector3         m_vel;
	float           m_birthTime;
    float           m_size;
    RgbaColour      m_colour;
	
    void Initialise(Vector3 const &pos, Vector3 const &vel);
	bool Advance();
	void Render(float _predictionTime);
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
