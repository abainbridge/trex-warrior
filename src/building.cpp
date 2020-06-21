#include "lib/universal_include.h"
#include "building.h"

#include "lib/hi_res_time.h"
#include "lib/matrix33.h"
#include "lib/matrix34.h"
#include "lib/resource.h"
#include "lib/gfx/shape.h"


Building::Building(Vector3 pos, Vector3 front)
: GameObj(ObjTypeBuilding, pos)
{
	m_front = front;
	m_shape = g_resourceManager.GetShape("building.shp");

    m_angVel.x = 6.0;
    m_angVel.y = 0.0001;
    m_orientation.SetToIdentity();

    m_momentOfInertia.Set(1.0, 1.0, 0.2);
}


void Building::Advance()
{
    m_orientation.FastRotateAround(m_orientation.f, m_angVel.x * g_advanceTime);
    m_orientation.FastRotateAround(m_orientation.u, m_angVel.y * g_advanceTime);
    m_orientation.FastRotateAround(m_orientation.r, m_angVel.z * g_advanceTime);
    m_orientation.Normalize();

    Vector3 deltaAngVel;
    deltaAngVel.x = ((m_momentOfInertia.y - m_momentOfInertia.z) * m_angVel.y * m_angVel.z) / m_momentOfInertia.x;
    deltaAngVel.y = ((m_momentOfInertia.z - m_momentOfInertia.x) * m_angVel.z * m_angVel.x) / m_momentOfInertia.y;
    deltaAngVel.z = ((m_momentOfInertia.x - m_momentOfInertia.y) * m_angVel.x * m_angVel.y) / m_momentOfInertia.z;

    m_angVel += deltaAngVel * g_advanceTime;
}


void Building::Render()
{
    Matrix34 mat(m_orientation.f, m_orientation.u, m_pos);
    m_shape->Render(0.0f, mat);
}
