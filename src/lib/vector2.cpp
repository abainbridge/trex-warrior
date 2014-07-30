#include "lib/universal_include.h"
#include "lib/vector2.h"

#include <math.h>

#include "vector3.h"


Vector2::Vector2(Vector3 const &v)
{ 
	x = v.x;
	y = v.z;
}


Vector2 Vector2::operator + (Vector2 const &b) const
{
	return Vector2(x + b.x, y + b.y);
}


Vector2 Vector2::operator - (Vector2 const &b) const
{
	return Vector2(x - b.x, y - b.y);
}


Vector2 Vector2::operator * (float const b) const
{
	return Vector2(x * b, y * b);
}


// Assign from a Vector3 - throws away Y value of Vector3
void Vector2::operator = (Vector3 const &b)
{
	x = b.x;
	y = b.z;
}


Vector2 const &Vector2::Normalize()
{
	float lenSqrd = x*x + y*y;
	if (lenSqrd > 0.0f)
	{
		float invLen = 1.0f / sqrtf(lenSqrd);
		x *= invLen;
		y *= invLen;
	}
	else
	{
		x = 0.0f;
        y = 1.0f;
	}

	return *this;
}


void Vector2::SetLen(float len)
{
	float scaler = len / Len();
	x *= scaler;
	y *= scaler;
}


float Vector2::Len() const
{
    return sqrtf(x * x + y * y);
}
