#ifndef INCLUDED_VECTOR2_H
#define INCLUDED_VECTOR2_H


class Vector3;


class Vector2
{
public:
	float x, y;

	Vector2()										{ x = y = 0.0f; }
	Vector2(Vector3 const &b);
	Vector2(float _x, float _y)						{ x=_x; y=_y; }

	void Set(float _x, float _y)					{ x=_x; y=_y; }

	Vector2 operator + (Vector2 const &b) const;
	Vector2 operator - (Vector2 const &b) const;
	Vector2 operator * (float const b) const;

	void	operator = (Vector2 const &b)			{ x=b.x; y=b.y; }
    void	operator = (Vector3 const &b);
	void	operator *= (float const b)				{ x*=b; y*=b; }
	void	operator += (Vector2 const &b)			{ x+=b.x; y+=b.y; }
	void	operator -= (Vector2 const &b)			{ x-=b.x; y-=b.y; }

	Vector2 const &Normalize();
	void	SetLen(float len);
	
    float	Len() const;
	float	LenSquared() const						{ return x*x + y*y; }
};


// Operator * between float and Vector2
inline Vector2 operator * (float scale, Vector2 const &v)
{
	return v * scale;
}


#endif
