#include "lib/universal_include.h"

#include <math.h>
#include <float.h>

#include "lib/vector2.h"
#include "lib/vector3.h"


Vector3 const g_upVector(0.0f, 1.0f, 0.0f);
Vector3 const g_zeroVector(0.0f, 0.0f, 0.0f);


void Vector3::RotateAroundX(float angle)
{
    FastRotateAround(Vector3(1,0,0), angle);
}


void Vector3::RotateAroundY(float angle)
{
    FastRotateAround(g_upVector, angle);
}


void Vector3::RotateAroundZ(float angle)
{
    FastRotateAround(Vector3(0,0,1), angle);
}


// ASSUMES that _norm is normalized
void Vector3::FastRotateAround(Vector3 const &norm, float angle)
{
	float dot = (*this) * norm;
	Vector3 a = norm * dot;
	Vector3 n1 = *this - a;
	Vector3 n2 = norm.CrossProduct(n1);
	float s = sinf(angle);
	float c = cosf(angle);

	*this = a + c*n1 + s*n2;
}


void Vector3::RotateAround(Vector3 const &axis)
{
	float angle = axis.LenSquared();
	if (angle < 1e-8) return;
	angle = sqrtf(angle);
	Vector3 norm(axis / angle);
	FastRotateAround(norm, angle);
}
