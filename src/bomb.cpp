#include "lib/universal_include.h"
#include "bomb.h"

#include "lib/sound/sound_system.h"
#include "lib/resource.h"
#include "big_explosions.h"
#include "level.h"


Bomb::Bomb(Vector3 const &pos)
: GameObj(ObjTypeBomb, pos)
{
    m_shape = g_resourceManager.GetShape("bomb.shp");
    m_shields = 1.0f;
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
