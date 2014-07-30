#include "lib/universal_include.h"
#include "building.h"

#include "lib/gfx/shape.h"
#include "lib/resource.h"


Building::Building(Vector3 pos, Vector3 front)
: GameObj(ObjTypeBuilding, pos)
{
	m_front = front;
	m_shape = g_resourceManager.GetShape("building.shp");
}


void Building::Render()
{
	Matrix34 mat(m_front, Vector3(0,1,0), m_pos);
	m_shape->Render(0.0f, mat);
}
