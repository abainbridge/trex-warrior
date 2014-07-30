#include "lib/universal_include.h"
#include "ship_wasp.h"

#include "lib/hi_res_time.h"
#include "lib/resource.h"
#include "app.h"
#include "missile.h"
#include "level.h"
#include "navigation.h"
#include "ship_player.h"


#define WASP_HOVER_HEIGHT 40.0f
#define WASP_SPEED 70.0f
#define WASP_TARGET_DISTANCE 125.0f // Wasp maneuvers until it is this distance from the target


Wasp::Wasp(Vector3 const &pos)
: Ship(ObjTypeWasp, pos)
{
	m_pos.y = WASP_HOVER_HEIGHT;

	m_shape = g_resourceManager.GetShape("wasp.shp");

	m_nextStateChangeTime = g_gameTime + 2.0f;
	m_state = StateMoving;
}


void Wasp::Advance()
{
	Vector3 toTarget = g_level->m_playerShip->m_pos - m_pos;
	toTarget.y = 0;
	float dist = toTarget.Len();
	if (dist < WASP_TARGET_DISTANCE)
		m_state = StateShooting;
	else
		m_state = StateMoving;
		
	if (m_state == StateMoving)
	{
		m_front = toTarget;
		m_front.y = 0;
		m_front.Normalize();

		m_pos += m_front * WASP_SPEED * g_advanceTime;
	}
	else
	{
		m_front = g_level->m_playerShip->m_pos - m_pos;
		m_front.y = 0.0f;
		m_front.Normalize();
		if (g_gameTime > m_nextFireTime)
		{
			m_nextFireTime = g_gameTime + 8.0f;
			Vector3 toPlayer = g_level->m_playerShip->m_pos - m_pos;
			if (rand() & 1)
				toPlayer.RotateAroundY(1.9f);
			else
				toPlayer.RotateAroundY(-1.9f);
			toPlayer.Normalize();
			g_level->AddObj(new Missile(this, g_level->m_playerShip, m_pos, toPlayer));
		}
	}
}
