#include "lib/universal_include.h"
#include "shield_powerup.h"

#include "lib/gfx/shape.h"
#include "lib/sound/sound_system.h"
#include "lib/resource.h"
#include "level.h"
#include "ship_player.h"


ShieldPowerup::ShieldPowerup(Vector3 const &pos)
: GameObj(ObjTypeShieldPowerup, pos)
{
    m_shape = g_resourceManager.GetShape("shield_powerup.shp");
}


void ShieldPowerup::Advance()
{
    SpherePackage sphere(m_pos, 10.0f);
    if (g_level->SphereHit(&sphere, ObjTypePlayerShip, NULL, NULL))
    {
        g_soundSystem->PlayWave("powerup_collected.wav", &m_pos);
        g_level->m_playerShip->IncreaseShields();
        g_level->DeleteObj(this);
    }
}


RgbaColour ShieldPowerup::GetRadarColour()
{
    return RgbaColour(255, 228, 0);
}
