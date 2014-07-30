#include "lib/universal_include.h"
#include "lib/plane.h"

#include "lib/vector3.h"


// Three points
Plane::Plane(Vector3 const &a, Vector3 const &b, Vector3 const &c)
{
	Vector3 vectInPlane1 = a - b;
	Vector3 vectInPlane2 = a - c;
	m_normal = vectInPlane1.CrossProduct(vectInPlane2);
	m_normal.Normalize();
	m_distFromOrigin = -m_normal * a;
}


// Returns the Cartesian definition of the plane in the form:
//   ax + by + cz + d = 0
void Plane::GetCartesianDefinition(float *a, float *b, float *c, float *d) const
{
	*a = m_normal.x;
	*b = m_normal.y;
	*c = m_normal.z;
	*d = m_distFromOrigin;
}
