#include "lib/universal_include.h"
#include "bullet.h"

#include "lib/gfx/debug_render.h"
#include "lib/gfx/simple_primitives.h"
#include "lib/gfx/shape.h"
#include "lib/sound/sound_system.h"
#include "lib/hi_res_time.h"
#include "lib/resource.h"
#include "app.h"
#include "arena.h"
#include "camera.h"
#include "ship.h"
#include "level.h"


Bullet::Bullet(GameObj *owner, Vector3 const &pos, Vector3 const &front)
: GameObj(ObjTypeBullet, pos)
{
	m_owner = owner;
	m_vel = front * 1000.0f;
	g_soundSystem->PlaySound("bloop.sh", &pos);
}


void Bullet::Advance()
{
	// Bullets move so fast that they can jump from one side of an object to 
	// the other in a single frame advance. When that happens, no collision
	// with the object will be detected. To prevent this we divide the frame
	// advance time and advance the bullet in many small steps.
	int numSlices = 10;
	float advanceTime = g_advanceTime / (float)numSlices;
	for (int slice = 0; slice < numSlices; slice++)
	{
		m_pos += m_vel * advanceTime;

		// Has bullet escaped the arena
		if (!g_level->m_arena->IsInside(m_pos))
			g_level->DeleteObj(this);

		// Hit check this bullet against other game objects in the location
		int numObjs = g_level->m_objects.Size();
		for (int i = 0; i < numObjs; i++)
		{
			GameObj *o = g_level->m_objects[i];
			if (o == this || o == m_owner)
				continue;

			if (!o->m_shape)
				continue;

			if (o->m_type == ObjTypeBullet || o->m_type == ObjTypeJumpPad)
				continue;

			SpherePackage sp(m_pos, BULLET_SIZE * 0.9f);
			Matrix34 osMat(o->m_front, g_upVector, o->m_pos);	
			if (o->m_shape->SphereHit(&sp, osMat, true))
			{
				// Bullet has hit something, destroy the bullet
				g_level->DeleteObj(this);
                
                g_soundSystem->PlaySound("crash.sh", &m_pos);

				o->Hit(1.0f);

				return;	// This bullet is dead now. No need to do the rest of the time slices.
			}
		}
	}
}


void Bullet::Render()
{
	int tid = g_resourceManager.GetTexture("bullet.bmp");
	Render3dSprite(g_app->m_camera->m_up, g_app->m_camera->GetRight(), m_pos,
				   BULLET_SIZE, BULLET_SIZE, tid, RgbaColour(255,0,0,255));
}
