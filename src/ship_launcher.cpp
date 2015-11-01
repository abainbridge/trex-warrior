#include "lib/universal_include.h"
#include "ship_launcher.h"

#include "lib/hi_res_time.h"
#include "lib/resource.h"
#include "app.h"
#include "level.h"
#include "missile.h"
#include "navigation.h"
#include "ship_player.h"


#define LAUNCHER_HOVER_HEIGHT 0.4f


Launcher::Launcher(Vector3 const &pos)
: Ship(ObjTypeLauncher, pos)
{
	m_pos.y = LAUNCHER_HOVER_HEIGHT;

	m_shape = g_resourceManager.GetShape("launcher.shp");
	m_navRouteFollower = new NavRouteFollower;
	m_navRouteFollower->pos = pos;
	m_navRouteFollower->maxSpeed = 90.0f;
	m_navRouteFollower->targetWayPointIdx = 0;

	m_nextStateChangeTime = g_gameTime + 2.0f;
	m_state = StateMoving;
}


void Launcher::Advance()
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
			m_nextFireTime += 8.0f;
            Vector3 pos = m_pos;
            pos.y += 15.0f;
			g_level->AddObj(new Missile(this, g_level->m_playerShip, pos, m_front));
		}
	}
}
