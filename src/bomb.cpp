#include "lib/universal_include.h"
#include "bomb.h"

#include "lib/resource.h"


Bomb::Bomb(Vector3 const &pos)
: GameObj(ObjTypeBomb, pos)
{
    m_shape = g_resourceManager.GetShape("bomb.shp");
}


void Bomb::TakeHit(float force)
{
}


RgbaColour Bomb::GetRadarColour()
{
    return RgbaColour(255, 128, 0);
}
