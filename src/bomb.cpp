#include "lib/universal_include.h"
#include "bomb.h"

#include "lib/gfx/shape.h"
#include "lib/sound/sound_system.h"
#include "lib/resource.h"
#include "big_explosions.h"
#include "level.h"
#include "ship.h"


Bomb::Bomb(Vector3 const &pos)
: GameObj(ObjTypeBomb, pos)
{
    m_shape = g_resourceManager.GetShape("bomb.shp");
    m_shields = 1.0f;
}


void Bomb::Advance()
{
    Vector3 pos = m_pos;
    pos.y += 2.0f;
    SpherePackage sp(pos, 4.0f);

    int numObjs = g_level->m_objects.Size();
    for (int i = 0; i < numObjs; i++)
    {
        GameObj *o = g_level->m_objects[i];
        if (o->m_type != ObjTypePlayerShip &&
            o->m_type != ObjTypeSpeedy)
            continue;

        Ship *ship = (Ship*)o;
        if (ship->m_speed < 90.0f)
            continue;

        Matrix34 osMat(o->m_front, g_upVector, o->m_pos);	
        if (o->m_shape->SphereHit(&sp, osMat, true))
        {
            // Something hit us
            TakeHit(m_shields + 1.0f);
        }
    }
}


void Bomb::TakeHit(float force)
{
    m_shields -= force;

    if (m_shields <= 0)
    {
        g_soundSystem->PlayWave("explosion.wav", &m_pos);
        m_pos.y += 10.0f;
        g_level->AddObj(new Boom(m_pos));
        g_level->DeleteObj(this);
    }
}


RgbaColour Bomb::GetRadarColour()
{
    return RgbaColour(255, 128, 0);
}
