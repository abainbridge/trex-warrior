#include "lib/universal_include.h"
#include "matrix33.h"

#include <math.h>
#include <memory.h>

#include "lib/debug_utils.h"



Matrix33::Matrix33(int ignored)
{
	SetToIdentity();
}


Matrix33::Matrix33(float rx, float ry, float rz, float ux, float uy, float uz, float fx, float fy, float fz)
:	r(rx,ry,rz),
	u(ux,uy,uz),
	f(fx,fy,fz)
{
}


void Matrix33::RotateAroundX(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);

	float t;

	t = r.y * c + r.z * s;
	r.z = r.y * -s + r.z * c;
	r.y = t;

	t = u.y * c + u.z * s;
	u.z = u.y * -s + u.z * c;
	u.y = t;

	t = f.y * c + f.z * s;
	f.z = f.y * -s + f.z * c;
	f.y = t;
}


void Matrix33::RotateAroundY(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);

	float t;

	t = r.z * c + r.x * s;
	r.x = r.z * -s + r.x * c;
	r.z = t;

	t = u.z * c + u.x * s;
	u.x = u.z * -s + u.x * c;
	u.z = t;

	t = f.z * c + f.x * s;
	f.x = f.z * -s + f.x * c;
	f.z = t;
}


void Matrix33::RotateAroundZ(float angle)
{
	float c = (float)cos(angle);
	float s = (float)sin(angle);

	float t;

	t = r.x * c + r.y * s;
	r.y = r.x * -s + r.y * c;
	r.x = t;

	t = u.x * c + u.y * s;
	u.y = u.x * -s + u.y * c;
	u.x = t;

	t = f.x * c + f.y * s;
	f.y = f.x * -s + f.y * c;
	f.x = t;
}


// Assumes normal is already normalized
void Matrix33::FastRotateAround(Vector3 const &norm, float angle)
{
	float s = (float)sin(angle);
	float c = (float)cos(angle);

	float dot = r * norm;
	Vector3 a = norm * dot;
	Vector3 n1 = r - a;
	Vector3 n2 = norm.CrossProduct(n1);
	r = a + c*n1 + s*n2;

	dot = u * norm;
	a = norm * dot;
	n1 = u - a;
	n2 = norm.CrossProduct(n1);
	u = a + c*n1 + s*n2;

	dot = f * norm;
	a = norm * dot;
	n1 = f - a;
	n2 = norm.CrossProduct(n1);
	f = a + c*n1 + s*n2;
}


void Matrix33::SetToIdentity()
{
	memset(this, 0, sizeof(Matrix33));
	r.x = 1.0f;
	u.y = 1.0f;
	f.z = 1.0f;
}
