#include "lib/universal_include.h"
#include "ship_speedy.h"

#include "lib/hi_res_time.h"
#include "lib/resource.h"
#include "app.h"
#include "bullet.h"
#include "level.h"
#include "navigation.h"
#include "ship_player.h"


#define SPEEDY_HOVER_HEIGHT 4.0f


Speedy::Speedy(Vector3 const &pos)
: Ship(ObjTypeSpeedy, pos)
{
	m_pos.y = SPEEDY_HOVER_HEIGHT;

	m_shape = g_resourceManager.GetShape("speedy.shp");
	m_navRouteFollower = new NavRouteFollower;
	m_navRouteFollower->pos = pos;
	m_navRouteFollower->maxSpeed = 90.0f;
	m_navRouteFollower->targetWayPointIdx = 0;

	m_nextStateChangeTime = g_gameTime + 2.0f;
	m_state = StateMoving;
}


void Speedy::Advance()
{
	if (g_gameTime > m_nextStateChangeTime)
	{
		if (m_state == StateMoving)
		{
			m_state = StateShooting;
			m_nextStateChangeTime = g_gameTime + 6.0f;
			m_nextFireTime = g_gameTime + 1.0f;
		}
		else
		{
			m_state = StateMoving;
			m_nextStateChangeTime = g_gameTime + 16.0f;
		}
	}

	if (m_state == StateMoving)
	{
		m_navRouteFollower->pos = m_pos;
		g_level->m_navRoutes[0]->NextPos(m_navRouteFollower);
		m_pos += m_navRouteFollower->front * m_navRouteFollower->maxSpeed * g_advanceTime;
		m_front = m_navRouteFollower->front;
	}
	else
	{
		m_front = g_level->m_playerShip->m_pos - m_pos;
		m_front.y = 0.0f;
		m_front.Normalize();
		if (g_gameTime > m_nextFireTime)
		{
			m_nextFireTime += 0.9f;
			g_level->AddObj(new Bullet(this, m_pos, m_front));
		}
	}
}
