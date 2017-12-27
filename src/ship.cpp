#include "lib/universal_include.h"
#include "ship.h"

#include "lib/gfx/shape.h"
#include "lib/sound/sound_system.h"
#include "lib/hi_res_time.h"
#include "lib/input.h"
#include "lib/math_utils.h"
#include "lib/matrix34.h"
#include "lib/resource.h"
#include "app.h"
#include "camera.h"
#include "level.h"
#include "bullet.h"
#include "navigation.h"
#include "particle_system.h"


static float const MAX_ACCELERATION = 500.0f;



Ship::Ship(int type, Vector3 const &pos)
: GameObj(type, pos)
{
	m_type = type;
	m_front = Vector3(-1,0,0);
	m_shields = 10.0f;
	m_speed = 0.0f;
    m_targetSpeed = 0.0f;
    m_whackVel = g_zeroVector;
    m_rotateVel = 0.0f;
}


void Ship::TakeHit(float force)
{
	if (m_type == ObjTypePlayerShip)
	{
		g_app->m_camera->CreateShake(0.2f);
        g_soundSystem->PlayWave("player_hit.wav", &m_pos);
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			Vector3 vel(sfrand(50), 30 + frand(60), sfrand(50));
			Vector3 pos = m_pos;
			pos.x += vel.x * 0.3f;
			pos.z += vel.z * 0.3f;
			g_particleSystem.CreateParticle(pos, vel);
		}
	}

	m_shields -= force;

	if (m_shields <= 0)
	{
		if (m_type == ObjTypePlayerShip)
			g_app->m_requestedState = App::StateGameOver;
		else
		{
			g_level->DeleteObj(this);

			for (int i = 0; i < 80; i++)
			{
				Vector3 vel(sfrand(80), 20 + frand(120), sfrand(80));
				Vector3 pos = m_pos;
				pos.x += vel.x * 0.3f;
				pos.z += vel.z * 0.3f;
				g_particleSystem.CreateParticle(pos, vel);
			}
		}

        g_soundSystem->PlayWave("explosion.wav", &m_pos);
    }
}


void Ship::Advance()
{
    // Collisions with buildings and arena perimeter
    SpherePackage hitPackage(m_pos, 16.0f);
    GameObj *o = g_level->SphereHit(&hitPackage, ObjTypeArena | ObjTypeBuilding, NULL, NULL);
    if (o)
    {
        Vector3 fromCentre = m_pos - o->m_pos;
        fromCentre.Normalize();
        float whackHardness = fabsf(m_speed) + 10.0f;

        if (o->m_type == ObjTypeArena)
        {
            m_whackVel = fromCentre * whackHardness * -0.4f;
        }
        else if (o->m_type == ObjTypeBuilding)
        {
            m_whackVel = fromCentre * whackHardness * 0.6f;
            g_soundSystem->PlayWave("player_collide.wav", &m_pos);
        }

        m_pos += m_whackVel * 0.1f;
        m_speed = 0.0f;
    }

    float amountVFriction = g_advanceTime * 0.5f;
    float amountHFriction = g_advanceTime * 3.5f;
    m_whackVel.x *= 1.0f - amountHFriction;
    m_whackVel.y *= 1.0f - amountVFriction;
    m_whackVel.z *= 1.0f - amountHFriction;

    if (m_speed < m_targetSpeed)
    {
        m_speed += g_advanceTime * MAX_ACCELERATION;
        if (m_speed > m_targetSpeed) m_speed = m_targetSpeed;
    }
    else
    {
        m_speed -= g_advanceTime * MAX_ACCELERATION;
        if (m_speed < m_targetSpeed) m_speed = m_targetSpeed;
    }

    m_pos += m_whackVel * g_advanceTime;
    m_pos += m_front * m_speed * g_advanceTime;
}


void Ship::Render()
{
	if (m_type == ObjTypePlayerShip && g_app->m_camera->GetMode() != Camera::ModeDebug)
		return;

	if (m_shields > 0)
	{
		Matrix34 mat(m_front, Vector3(0,1,0), m_pos);
		m_shape->Render(0.0f, mat);
	}
}
