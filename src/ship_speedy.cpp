#include "lib/universal_include.h"
#include "ship_speedy.h"

#include <float.h>
#include "lib/gfx/debug_render.h"
#include "lib/gfx/shape.h"
#include "lib/sound/sound_system.h"
#include "lib/hi_res_time.h"
#include "lib/resource.h"
#include "app.h"
#include "bullet.h"
#include "level.h"
#include "ship_player.h"


#define DEBUG_SPEEDY 0

#if DEBUG_SPEEDY
#include "lib/input.h"
#define DEBUG_RENDER_SPHERE(pos, radius, col) DebugRenderSphere(pos, radius, col)
#define DEBUG_RENDER_LINE(start, end, col) DebugRenderLine(start, end, col)
#else
#define DEBUG_RENDER_SPHERE(a, b, c)
#define DEBUG_RENDER_LINE(a, b, c)
#endif


static float const FULL_SPEED = 150.0f;
static float const HOVER_HEIGHT = 4.0f;
static float const SHIP_WIDTH = 34.0f;
static int const HITCHECK_OBJ_TYPES = GameObj::ObjTypeBuilding | 
                                      GameObj::ObjTypeArena |
                                      GameObj::ObjTypeBomb;
static float LOOK_AHEAD_DIST = 70.0f;


Speedy::Speedy(Vector3 const &pos)
: Ship(ObjTypeSpeedy, pos)
{
	m_pos.y = HOVER_HEIGHT;
    m_speed = FULL_SPEED;

	m_shape = g_resourceManager.GetShape("speedy.shp");

	m_nextStateChangeTime = g_gameTime + 2.0f;
	m_state = StateMoving;
}


void Speedy::DoShooting()
{
    m_speed = 0.0f;
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


void Speedy::LeftAndRightRayHits(bool *leftHit, Vector3 *leftHitPos, bool *rightHit, Vector3 *rightHitPos)
{
    RayPackage leftRay(m_pos + (SHIP_WIDTH/2.0f) * GetRight(), m_front, LOOK_AHEAD_DIST);
    RayPackage rightRay(m_pos - (SHIP_WIDTH/2.0f) * GetRight(), m_front, LOOK_AHEAD_DIST);

    *leftHit = !!g_level->RayHit(&leftRay, HITCHECK_OBJ_TYPES, NULL, leftHitPos);
    *rightHit = !!g_level->RayHit(&rightRay, HITCHECK_OBJ_TYPES, NULL, rightHitPos);

    if (*leftHit)
        DEBUG_RENDER_LINE(leftRay.m_start, leftRay.m_end, RgbaColour(255, 0, 0));
    else
        DEBUG_RENDER_LINE(leftRay.m_start, leftRay.m_end, g_colourWhite);

    if (*rightHit)
        DEBUG_RENDER_LINE(rightRay.m_start, rightRay.m_end, RgbaColour(255, 0, 0));
    else
        DEBUG_RENDER_LINE(rightRay.m_start, rightRay.m_end, g_colourWhite);
}


void Speedy::DoMoving()
{
    m_steeringTorque = 0.0f;
    m_speed = FULL_SPEED;

    bool leftRayHit, rightRayHit;
    Vector3 leftRayHitPos, rightRayHitPos;
    LeftAndRightRayHits(&leftRayHit, &leftRayHitPos, &rightRayHit, &rightRayHitPos);

    if (leftRayHit && rightRayHit)
    {
        // Are we heading into a corner that we will get stuck in? Or are we 
        // trying to squeeze through a narrow gap. Let's find out by seeing
        // if there is a gap half way between the two hit positions.
        SpherePackage sphere((leftRayHitPos + rightRayHitPos) / 2.0f, SHIP_WIDTH / 2.0f);
        if (g_level->SphereHit(&sphere, HITCHECK_OBJ_TYPES, NULL, NULL))
        {
            DEBUG_RENDER_SPHERE(sphere.m_pos, sphere.m_radius, RgbaColour(255, 0, 0));

            m_state = StateAvoidingCorner;

            // It is hard to know which way to turn to avoid the corner. A simple
            // strategy that seems to work is to always turn towards the centre
            // of the arena, which is at pos (0,0,0).
            Vector3 frontCrossToCentre = m_pos;
            frontCrossToCentre.y = 0.0f;
            frontCrossToCentre = frontCrossToCentre.CrossProduct(m_front);
            m_steeringTorque = 0.02f * m_speed;
            if (frontCrossToCentre.y < 0.0f)
                m_steeringTorque = -m_steeringTorque;
        }
        else
        {
             DEBUG_RENDER_SPHERE(sphere.m_pos, sphere.m_radius, g_colourWhite);
        }
    }
    else if (leftRayHit)
    {
        float hitDist = (m_pos - leftRayHitPos).Len();
        if (hitDist > LOOK_AHEAD_DIST)
            hitDist = LOOK_AHEAD_DIST;
        m_steeringTorque += (LOOK_AHEAD_DIST - hitDist) * 0.002f * m_speed;
    }
    else if (rightRayHit)
    {
        float hitDist = (m_pos - rightRayHitPos).Len();
        if (hitDist > LOOK_AHEAD_DIST)
            hitDist = LOOK_AHEAD_DIST;
        m_steeringTorque -= (LOOK_AHEAD_DIST - hitDist) * 0.002f * m_speed;
    }
    else
    {
        SpherePackage sphere(m_pos + m_front * LOOK_AHEAD_DIST * 0.8f, SHIP_WIDTH/2.5f);
        if (g_level->SphereHit(&sphere, HITCHECK_OBJ_TYPES, NULL, NULL))
        {
            m_steeringTorque -= 0.008f * m_speed;
            DEBUG_RENDER_SPHERE(sphere.m_pos, sphere.m_radius, RgbaColour(255, 0, 0));
        }
        else
        {
            DEBUG_RENDER_SPHERE(sphere.m_pos, sphere.m_radius, g_colourWhite);
        }
    }

#if DEBUG_SPEEDY
    if (g_keys[KEY_LEFT])
        m_front.RotateAroundY(3.0f * g_advanceTime);
    if (g_keys[KEY_RIGHT])
        m_front.RotateAroundY(-3.0f * g_advanceTime);
    if (g_keyDeltas[KEY_SPACE])
        m_speed = FULL_SPEED - m_speed;

    if (g_keys[KEY_MINUS_PAD])
        g_advanceTime = g_advanceTime * 0.1f;
    if (g_keys[KEY_PLUS_PAD])
        g_advanceTime = g_advanceTime * 10.0f;
    
    if (m_speed > 0.1f)
#endif
    m_front.RotateAroundY(m_steeringTorque * g_advanceTime);
}


void Speedy::DoAvoidingCorner()
{
    m_front.RotateAroundY(m_steeringTorque * g_advanceTime);

    bool leftRayHit, rightRayHit;
    Vector3 leftRayHitPos, rightRayHitPos;
    LeftAndRightRayHits(&leftRayHit, &leftRayHitPos, &rightRayHit, &rightRayHitPos);

    float nearestHitDist = FLT_MAX;
    if (leftRayHit)
    {
        float hitDist = (leftRayHitPos - m_pos).Len();
        nearestHitDist = hitDist;
    }
    if (rightRayHit)
    {
        float hitDist = (rightRayHitPos - m_pos).Len();
        if (hitDist < nearestHitDist)
            nearestHitDist = hitDist;
    }

    if (nearestHitDist > LOOK_AHEAD_DIST)
        nearestHitDist = LOOK_AHEAD_DIST;
    nearestHitDist -= SHIP_WIDTH;
    if (nearestHitDist < 0.0f)
        nearestHitDist = 0.0f;
    float maxDist = LOOK_AHEAD_DIST - SHIP_WIDTH;

    m_speed = FULL_SPEED * nearestHitDist / maxDist;

    if (!leftRayHit && !rightRayHit)
        m_state = StateMoving;
}


void Speedy::Advance()
{
	if (g_gameTime > m_nextStateChangeTime)
	{
		if (m_state == StateMoving || m_state == StateAvoidingCorner)
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

	switch (m_state)
    {
    case StateMoving: DoMoving(); break;
    case StateShooting: DoShooting(); break;
    case StateAvoidingCorner: DoAvoidingCorner(); break;
	}

    Ship::Advance();
}


void Speedy::Render()
{
	if (m_shields > 0)
	{
        Vector3 up = g_upVector;
        up.RotateAround(m_front * m_steeringTorque * -0.1f);

		Matrix34 mat(m_front, up, m_pos);
		m_shape->Render(0.0f, mat);
	}
}
