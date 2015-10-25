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
#define PARTICLE_SIZE (40.0f)
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
	
    static RgbaColour colours[] = 
    {
        RgbaColour(255, 255, 255),
        RgbaColour(70, 70, 70),
        RgbaColour(140, 140, 140),
        RgbaColour(33, 70, 70),
        RgbaColour(60, 140, 140),
        RgbaColour(140, 210, 210),
    };

    const unsigned numColours = sizeof(colours) / sizeof(colours[0]);
    m_colour = colours[rand() % numColours];
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


void Particle::Render(float predictionTime)
{
	double deathTime = m_birthTime + PARTICLE_LIFE;
    float midTime = m_birthTime + PARTICLE_LIFE * 0.5f;

    RgbaColour midColour = m_colour;
    midColour.a = 180;
    RgbaColour colour;

    if (g_gameTime < midTime)
    {
        RgbaColour startColour = RgbaColour(255, 255, 255, 180);
        float fractionOfStartColour = (midTime - g_gameTime) / (midTime - m_birthTime);
        if (fractionOfStartColour > 1.0f)
            fractionOfStartColour = 0.0f;
        colour = startColour * fractionOfStartColour + midColour * (1.0f - fractionOfStartColour); 
    }
    else
    {                
        RgbaColour endColour = midColour;
        endColour.a = 0;
        float fractionOfMidColour = (deathTime - g_gameTime) / (deathTime - midTime);
        if (fractionOfMidColour > 1.0f)
            fractionOfMidColour = 0.0f;
        colour = midColour * fractionOfMidColour + endColour * (1.0f - fractionOfMidColour);
    }
                  
    Vector3 predictedPos = m_pos + predictionTime * m_vel;
    float distToParticle = (g_app->m_camera->m_pos - predictedPos).Len();			
    float size = PARTICLE_SIZE / sqrtf(distToParticle);
	Vector3 up(g_app->m_camera->m_up * size);
	Vector3 right(g_app->m_camera->GetRight() * size);

    glColor4ubv(colour.GetData());

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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GlHelperDisable(GL_LIGHTING);
	GlHelperEnable(GL_BLEND);
	GlHelperEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_resourceManager.GetTexture("bullet.bmp", true));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glDepthMask(false);
    
  	int size = m_particles.Size();

    for (int i = 0; i < size; i++)
	{
        if (m_particles.IsIndexUsed(i))
        {
            Particle *p = m_particles.GetPointer(i);
		    p->Render(g_predictionTime);
        }
	}

    glDepthMask(true);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    END_PROFILE(g_app->m_profiler, "Render Particles");
}


void ParticleSystem::Empty()
{
}
