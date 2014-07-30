#ifndef INCLUDED_MATRIX33_H
#define INCLUDED_MATRIX33_H


#include "vector3.h"

class Matrix33
{
public:
	Vector3 r, u, f;

	Matrix33() {}
	Matrix33(int ignored);
	inline Matrix33(float rx, float ry, float rz, float ux, float uy, float uz, float fx, float fy, float fz);
	
	void RotateAroundX(float angle);
	void RotateAroundY(float angle);
	void RotateAroundZ(float angle);
	void FastRotateAround(Vector3 const & norm, float angle);
	void SetToIdentity();
};


// Operator * between vector3 and matrix33
inline Vector3 operator * (Vector3 const &v, Matrix33 const &m)
{
	return Vector3(m.r.x * v.x + m.r.y * v.y + m.r.z * v.z,
				   m.u.x * v.x + m.u.y * v.y + m.u.z * v.z,
				   m.f.x * v.x + m.f.y * v.y + m.f.z * v.z);
}


#endif

