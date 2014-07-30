#include "lib/universal_include.h"
#include "particle_system.h"

#include <math.h>

#include "lib/gfx/opengl_helper.h"
#include "lib/debug_utils.h"
#include "lib/hi_res_time.h"
#include "lib/math_utils.h"
#include "lib/profiler.h"
#include "lib/resource.h"

#include "app.h"
#include "camera.h"
#include "particle_system.h"


#define PARTICLE_LIFE (5.0f)
#define PARTICLE_SIZE (50.0f)
#define PARTICLE_GRAVITY (140.0f)
#define PARTICLE_FRICTION (0.6f)


ParticleSystem g_particleSystem;


// ****************************************************************************
// Particle
// ****************************************************************************

void Particle::Initialise(Vector3 const &pos, Vector3 const &vel)
{
	m_pos = pos;
	m_vel = vel;
	m_colour = g_colourWhite;  
    m_size = PARTICLE_SIZE;
	m_birthTime = g_gameTime;
}


// Returns true if this particle should be removed from the particle list
bool Particle::Advance()
{   
	double timeToDie = m_birthTime + PARTICLE_LIFE;
	if (g_gameTime > timeToDie)
		return true;

	m_pos += m_vel * g_advanceTime;
	
	// Friction
	float amount = g_advanceTime * PARTICLE_FRICTION;
	m_vel *= (1.0f - amount);

	// Bounce off ground
    float groundHeight = 2.0f;
    if (m_pos.y < groundHeight)
    {
        m_pos.y = groundHeight;
		m_vel.y = -m_vel.y * 0.5f;
    }        
	else if (m_pos.y > groundHeight)
	{
		// Gravity
		m_vel.y -= g_advanceTime * PARTICLE_GRAVITY;
	}
    
    return false;
}


void Particle::Render(float _predictionTime)
{
    double birthTime = m_birthTime;
	double deathTime = m_birthTime + PARTICLE_LIFE;
    float startFade = birthTime + PARTICLE_LIFE * 0.5f;
    int alpha;
    if (g_gameTime < startFade)
    {
        alpha = 90;
    }
    else
    {                
        float fractionFade = (g_gameTime - startFade) / (deathTime - startFade);
        alpha = 90 - 90 * fractionFade;
        if (alpha < 0) 
			alpha = 0;
    }
                  
    Vector3 predictedPos = m_pos + _predictionTime * m_vel;
    float distToParticle = (g_app->m_camera->m_pos - predictedPos).Len();			
    float size = m_size / sqrtf(distToParticle);
	Vector3 up(g_app->m_camera->m_up * size);
	Vector3 right(g_app->m_camera->GetRight() * size);

    glColor4ub(m_colour.r, m_colour.g, m_colour.b, alpha);            

    glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
        glVertex3fv( (predictedPos - up).GetData() );

		glTexCoord2i(0, 1);
        glVertex3fv( (predictedPos + right).GetData() );
        
		glTexCoord2i(1, 1);
		glVertex3fv( (predictedPos + up).GetData() );
        
		glTexCoord2i(1, 0);
		glVertex3fv( (predictedPos - right).GetData() );
    glEnd();
}


// ****************************************************************************
// ParticleSystem
// ****************************************************************************

void ParticleSystem::CreateParticle(Vector3 const &pos, Vector3 const &vel)
{
	Particle *aParticle = m_particles.GetFree();
    aParticle->Initialise(pos, vel);
}


void ParticleSystem::Advance()
{   
    START_PROFILE(g_app->m_profiler, "Advance Particles");

    int numParticles = m_particles.Size();
    for (int i = 0; i <= numParticles; i++)
    {
        if (m_particles.IsIndexUsed(i))
        {
            Particle *p = m_particles.GetPointer(i);
			if (p->Advance())
				m_particles.MarkNotUsed(i);
		}
    }

    END_PROFILE(g_app->m_profiler, "Advance Particles");
}


void ParticleSystem::Render()
{
    START_PROFILE(g_app->m_profiler, "Render Particles");

    GlHelperDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	GlHelperDisable(GL_LIGHTING);
	GlHelperEnable(GL_BLEND);
	GlHelperEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_resourceManager.GetTexture("bullet.bmp"));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glDepthMask(false);
    
	// Render all the particles that are up-to-date with server advances
  	int size = m_particles.Size();

    for (int i = 0; i < size; i++)
	{
        if (m_particles.IsIndexUsed(i))
        {
            Particle *p = m_particles.GetPointer(i);
		    p->Render(g_predictionTime);
        }
	}

    glDepthMask(true );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    END_PROFILE(g_app->m_profiler, "Render Particles");
}


void ParticleSystem::Empty()
{
}
