#include "lib/universal_include.h"
#include "ship_speedy.h"

#include "lib/gfx/debug_render.h"
#include "lib/gfx/shape.h"
#include "lib/sound/sound_system.h"
#include "lib/hi_res_time.h"
#include "lib/resource.h"
#include "app.h"
#include "bullet.h"
#include "level.h"
#include "ship_player.h"


#define SPEEDY_HOVER_HEIGHT 4.0f


Speedy::Speedy(Vector3 const &pos)
: Ship(ObjTypeSpeedy, pos)
{
	m_pos.y = SPEEDY_HOVER_HEIGHT;
    m_speed = 40.0f;

	m_shape = g_resourceManager.GetShape("speedy.shp");

	m_nextStateChangeTime = g_gameTime + 2.0f;
	m_state = StateMoving;
}

#include "lib/input.h"
void Speedy::DoMoving()
{
    float steeringTorque = 0.0f;
    float speed = m_speed;

    SpherePackage spherePackage(m_pos + m_front * 20.0f, 35.0f);
    DebugRenderSphere(spherePackage.m_pos, spherePackage.m_radius);

    int numObjs = g_level->m_objects.Size();
    for (int i = 0; i < numObjs; i++)
    {
        GameObj *o = g_level->m_objects[i];
        if (o->m_type != ObjTypeBuilding && o->m_type != ObjTypeArena && o->m_type != ObjTypePlayerShip)
            continue;

        Matrix34 osMat(o->m_front, g_upVector, o->m_pos);	
        Vector3 hitPos;
        if (o->m_shape->SphereHit(&spherePackage, osMat, true, &hitPos))
        {
            Vector3 toHitPos = hitPos - m_pos;
            toHitPos.y = 0.0f;
            Vector3 frontCrossToHit = toHitPos.CrossProduct(m_front);

            float toHitPosLen = toHitPos.Len();
            toHitPos += m_front;
            if (toHitPos.Len() > toHitPosLen)
            {
                DebugRenderSphere(hitPos, 5.0f, RgbaColour(0,255,255));
                float torqueForThisHit = 0.3f / frontCrossToHit.y;
                steeringTorque += torqueForThisHit;
                speed /= 1.0f + fabsf(torqueForThisHit) * 20.0f;
            }
            else
            {
                DebugRenderSphere(hitPos, 5.0f, RgbaColour(255,0,255));
            }
        }
    }

    if (g_keys[KEY_LEFT])
        steeringTorque += 0.03f;
    if (g_keys[KEY_RIGHT])
        steeringTorque -= 0.03f;
    if (g_keyDeltas[KEY_SPACE])
        m_speed = 40.0f - m_speed;

    if (g_keys[KEY_MINUS_PAD])
        g_advanceTime = g_advanceTime * 0.1f;
    if (g_keys[KEY_PLUS_PAD])
        g_advanceTime = g_advanceTime * 10.0f;
    
    if (m_speed > 0.1f)
        m_front.RotateAroundY(steeringTorque);

    m_pos += m_front * speed * g_advanceTime;
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

	if (1 || m_state == StateMoving)
	{
        DoMoving();
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
            g_soundSystem->PlayWave("enemy_shoot.wav", &m_pos);
		}
	}
}
