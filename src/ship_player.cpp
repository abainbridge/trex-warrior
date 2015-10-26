#include "lib/universal_include.h"
#include "ship_player.h"

#include "lib/gfx/shape.h"
#include "lib/hi_res_time.h"
#include "lib/input.h"
#include "lib/sound/sound_system.h"
#include "bullet.h"
#include "level.h"


ShipPlayer::ShipPlayer(Vector3 const &pos)
: Ship(ObjTypePlayerShip, pos)
{
	m_laserTemp = 0.0f;
	m_maxSpeed = PLAYER_SHIP_MAX_MAX_SPEED / 2.0f;
	m_shields = PLAYER_SHIP_MAX_SHIELDS;
	m_wackVel = g_zeroVector;
	m_rotateVel = 0.0f;
}


void ShipPlayer::Advance()
{
	if (m_shields < 0)
		return;


	//
	// Take user input

	if (g_inputManager->m_rmb)
	{
		m_speed -= g_inputManager->m_mouseVelY;
		clamp(m_speed, -30.0f, m_maxSpeed);
	}

	if (g_keys[KEY_SPACE])
		m_speed = 0.0f;

	if (g_inputManager->m_lmbClicked && m_laserTemp < 1.0f)
	{
		Vector3 pos = m_pos + m_front * 10.0f;
		pos.y += 2.0f;
		Bullet *bul = new Bullet(this, pos, m_front);
        g_soundSystem->PlayWave("player_shoot.wav", &pos);
		g_level->AddObj(bul);

		m_laserTemp += 0.08;
	}

	float temp = 1.0f - (g_advanceTime * 3.5f);
	m_rotateVel *= temp;	// Rotational friction
	m_rotateVel += g_inputManager->m_mouseVelX * g_advanceTime * -0.04f;
	m_front.RotateAroundY(m_rotateVel);

	
	//
	// Physics

	float amountVFriction = g_advanceTime * 0.5f;
	float amountHFriction = g_advanceTime * 3.5f;
	m_wackVel.x *= 1.0f - amountHFriction;
	m_wackVel.y *= 1.0f - amountVFriction;
	m_wackVel.z *= 1.0f - amountHFriction;

	m_pos += m_wackVel * g_advanceTime;
	m_pos += m_front * m_speed * g_advanceTime;

	if (m_pos.y < PLAYER_SHIP_HOVER_HEIGHT)
	{
		bool onJumpPad = false;
		for (int i = 0; i < g_level->m_objects.Size(); i++)
		{
			GameObj *o = g_level->m_objects[i];
			if (o->m_type == ObjTypeJumpPad)
			{
				Vector3 toPad = o->m_pos - m_pos;
				toPad.y = 0.0f;
				if (toPad.Len() < 22.0f)
					onJumpPad = true;
			}
		}
	
		if (onJumpPad)
        {
			m_wackVel.y = 85.0f;
            g_soundSystem->PlayWave("jump_pad.wav", &m_pos);
        }
		else
        {
			m_wackVel.y *= -0.5f;
        }

		m_pos.y = PLAYER_SHIP_HOVER_HEIGHT;
	}

	// Collisions with buildings and arena perimeter
	SpherePackage hitPackage(m_pos, 20.0f);
	for (int i = 0; i < g_level->m_objects.Size(); i++)
	{
		GameObj *o = g_level->m_objects[i];
		if (o->m_type == ObjTypeArena)
		{
			Matrix34 osMat(o->m_front, g_upVector, o->m_pos);	
			if (o->m_shape->SphereHit(&hitPackage, osMat, true))
			{
				Vector3 fromCentre = m_pos - o->m_pos;
				fromCentre.Normalize();
				m_wackVel = fromCentre * (m_speed * -0.4f);
				m_pos += m_wackVel * 0.1f;
				m_speed = 0.0f;
			}
		}
		else if (o->m_type == ObjTypeBuilding)
		{
			Matrix34 osMat(o->m_front, g_upVector, o->m_pos);	
			if (o->m_shape->SphereHit(&hitPackage, osMat, true))
			{
				Vector3 fromCentre = m_pos - o->m_pos;
				fromCentre.Normalize();
				m_wackVel = fromCentre * (m_speed * 0.6f);
				m_pos += m_wackVel * 0.1f;
				m_speed = 0.0f;

                g_soundSystem->PlayWave("player_collide.wav", &m_pos);
			}
		}
	}

	// Gravity
	m_wackVel.y -= g_advanceTime * 9.81 * 8.0f;

	m_laserTemp -= 0.08 * g_advanceTime;
	if (m_laserTemp < 0.0f)
		m_laserTemp = 0.0f;
}
