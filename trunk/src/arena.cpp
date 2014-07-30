#include "lib/universal_include.h"
#include "arena.h"

#include "lib/gfx/shape.h"
#include "lib/resource.h"


Arena::Arena(Vector3 pos, Vector3 front)
: GameObj(ObjTypeArena, pos)
{
	m_front = front;
	m_shape = g_resourceManager.GetShape("arena1_walls.shp");
	m_floor = g_resourceManager.GetShape("arena1_floor.shp");
}


bool Arena::IsInside(Vector3 pos)
{
	pos.y = 0.0f;
	if (pos.Len() > 400.0f)
		return false;
	return true;
}


void Arena::Render()
{
	Matrix34 mat(m_front, Vector3(0,1,0), m_pos);
	m_shape->Render(0.0f, mat);
	m_floor->Render(0.0f, mat);
}
