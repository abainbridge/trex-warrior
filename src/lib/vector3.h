#ifndef INCLUDED_VECTOR3_H
#define INCLUDED_VECTOR3_H


#include "lib/math_utils.h"
#include "lib/vector2.h"

class Vector3;

extern Vector3 const g_upVector;
extern Vector3 const g_zeroVector;


class Vector3
{
public:
	float x, y, z;

	Vector3() {	x = y = z = 0.0f; }
	Vector3(float _x, float _y, float _z) { x=_x; y=_y; z=_z; }
	Vector3(Vector2 const &b) { x=b.x; y=0.0f; z=b.y; }

	void Set(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3 CrossProduct(Vector3 const &b) const
	{
		return Vector3(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x); 
	}

	float operator * (Vector3 const &b) const
	{
		return x*b.x + y*b.y + z*b.z;
	}

	Vector3 operator - () const
	{
		return Vector3(-x, -y, -z);
	}

	Vector3 operator + (Vector3 const &b) const
	{
		return Vector3(x + b.x, y + b.y, z + b.z);
	}

	Vector3 operator - (Vector3 const &b) const
	{
		return Vector3(x - b.x, y - b.y, z - b.z);
	}

	Vector3 operator * (float const b) const
	{
		return Vector3(x * b, y * b, z * b);
	}

	Vector3 operator / (float const b) const
	{
		float multiplier = 1.0f / b;
		return Vector3(x * multiplier, y * multiplier, z * multiplier);
	}

	Vector3 const &operator = (Vector3 const &b)
	{
		x = b.x;
		y = b.y;
		z = b.z;
		return *this;
	}

	Vector3 const &operator *= (float const b)
	{
		x *= b;
		y *= b;
		z *= b;
		return *this;
	}

	Vector3 const &operator += (Vector3 const &b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
		return *this;
	}

	Vector3 const &operator -= (Vector3 const &b)
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		return *this;
	}

	Vector3 const &Normalize()
	{
		float lenSqrd = x*x + y*y + z*z;
		if (lenSqrd > 0.0f)
		{
			float invLen = 1.0f / sqrtf(lenSqrd);
			x *= invLen;
			y *= invLen;
			z *= invLen;
		}
		else
		{
			x = y = 0.0f;
			z = 1.0f;
		}

		return *this;
	}

	void SetLen(float _len)
	{
		float mag = Len();
		if (NearlyEquals(mag, 0.0f))
			x = _len;

		float scaler = _len / Len();
		*this *= scaler;
	}

	void RotateAroundX(float angle);
	void RotateAroundY(float angle);
	void RotateAroundZ(float angle);
	void FastRotateAround(Vector3 const &norm, float angle);
	void RotateAround(Vector3 const &norm);

	float Len() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float LenSquared() const
	{
		return x * x + y * y + z * z;
	}

	float const *GetData() const
	{
		return &x;
	}
};


// Operator * between float and Vector3
inline Vector3 operator * (float scale, Vector3 const &v)
{
	return v * scale;
}


#endif
