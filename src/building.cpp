#include "lib/universal_include.h"
#include "building.h"

#include "lib/resource.h"


Building::Building(Vector3 pos, Vector3 front)
: GameObj(ObjTypeBuilding, pos)
{
	m_front = front;
	m_shape = g_resourceManager.GetShape("building.shp");
}
