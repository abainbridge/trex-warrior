#include "lib/universal_include.h"
#include "missile.h"

#include "lib/gfx/debug_render.h"
#include "lib/gfx/simple_primitives.h"
#include "lib/gfx/shape.h"
#include "lib/sound/sound_system.h"
#include "lib/hi_res_time.h"
#include "lib/resource.h"
#include "arena.h"
#include "big_explosions.h"
#include "camera.h"
#include "ship.h"
#include "level.h"


#define MISSILE_HOVER_HEIGHT 8.0f
#define MISSILE_SPEED 160.0f
#define MISSILE_TURN_SPEED 2.15f


Missile::Missile(GameObj *owner, GameObj *target, Vector3 const &pos, Vector3 const &front)
: GameObj(ObjTypeMissile, pos)
{
	m_owner = owner;
	m_target = target;
	m_shields = 1.9f;
	m_front = front;
	m_shape = g_resourceManager.GetShape("missile.shp");
}


void Missile::Advance()
{
	// Missiles move so fast that they can jump from one side of an object to 
	// the other in a single frame advance. When that happens, no collision
	// with the object will be detected. To prevent this we divide the frame
	// advance time and advance the missile in many small steps.
	int numSlices = 10;
	float advanceTime = g_advanceTime / (float)numSlices;
	for (int slice = 0; slice < numSlices; slice++)
	{
		// Steer towards target in the horizontal plane
		Vector3 toTarg = m_target->m_pos - m_pos;
		toTarg.y = 0.0f;
		m_front.y = 0.0f;
		Vector3 crossProd = m_front.CrossProduct(toTarg);
		if (crossProd.Len() > MISSILE_TURN_SPEED)
			crossProd.SetLen(MISSILE_TURN_SPEED * advanceTime);
		m_front.RotateAround(crossProd);
		
		// Steer towards the target in the vertical plane
		float offsetFromIdealHeight = MISSILE_HOVER_HEIGHT - m_pos.y;
		m_front.y = signf(offsetFromIdealHeight) * sqrtf(fabs(offsetFromIdealHeight)) * 0.08f;
		m_front.Normalize();

		// Move
		m_pos += m_front * MISSILE_SPEED * advanceTime;

		// Hit check this missile against other game objects in the location
		int numObjs = g_level->m_objects.Size();
		for (int i = 0; i < numObjs; i++)
		{
			GameObj *o = g_level->m_objects[i];
			if (o == this || o == m_owner)
				continue;

			if (!o->m_shape)
				continue;

			if (o->m_type == ObjTypeJumpPad)
				continue;

			SpherePackage sp(m_pos, 2.2f);
			Matrix34 osMat(o->m_front, g_upVector, o->m_pos);	
			if (o->m_shape->SphereHit(&sp, osMat, true))
			{
				// Missile has hit something, destroy the missile
				if (o->m_type == ObjTypePlayerShip)
                    g_level->DeleteObj(this);
                else
                    TakeHit(m_shields + 1.0f);

                // Give damage to whatever we hit
				o->TakeHit(4.0f);

				return;	// This missile is dead now. No need to do the rest of the time slices.
			}
		}
	}
}


void Missile::Render()
{
	Matrix34 mat(m_front, Vector3(0,1,0), m_pos);
	m_shape->Render(0.0f, mat);
}


void Missile::TakeHit(float force)
{
	m_shields -= force;

	if (m_shields <= 0)
    {
        g_soundSystem->PlayWave("missile_destroyed.wav", &m_pos);
        g_level->AddObj(new MissileDeath(m_pos));
        g_level->DeleteObj(this);
    }
}
