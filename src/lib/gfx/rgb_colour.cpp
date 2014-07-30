#include "lib/universal_include.h"

#include "rgb_colour.h"

RgbaColour g_colourBlack(0,0,0);
RgbaColour g_colourWhite(255,255,255);


RgbaColour::RgbaColour(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
    Set( _r, _g, _b, _a );
}


RgbaColour::RgbaColour(int _col)
{
    Set(_col);
}


void RgbaColour::Set(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}


void RgbaColour::Set(int _col)
{
    r = (_col & 0xff000000) >> 24;
    g = (_col & 0x00ff0000) >> 16;
    b = (_col & 0x0000ff00) >> 8;
    a = (_col & 0x000000ff) >> 0;
}


RgbaColour RgbaColour::operator + (RgbaColour const &_b) const
{
	return RgbaColour((r + _b.r),
					  (g + _b.g),
					  (b + _b.b),
					  (a + _b.a));
}


RgbaColour RgbaColour::operator - (RgbaColour const &_b) const
{
	return RgbaColour((r - _b.r), 
					  (g - _b.g), 
					  (b - _b.b),
					  (a - _b.a));
}


RgbaColour RgbaColour::operator * (float const _b) const
{
	return RgbaColour((unsigned char) ((float)r * _b),
					  (unsigned char) ((float)g * _b),
					  (unsigned char) ((float)b * _b),
					  (unsigned char) ((float)a * _b));
}


RgbaColour RgbaColour::operator / (float const _b) const
{
	float multiplier = 1.0f / _b;
	return RgbaColour((unsigned char) ((float)r * multiplier),
					  (unsigned char) ((float)g * multiplier),
					  (unsigned char) ((float)b * multiplier),
					  (unsigned char) ((float)a * multiplier));
}


RgbaColour const &RgbaColour::operator = (RgbaColour const &_b)
{
	r = _b.r;
	g = _b.g;
	b = _b.b;
	a = _b.a;
	return *this;
}


RgbaColour const &RgbaColour::operator *= (float const _b)
{
	r = (unsigned char)((float)r * _b);
	g = (unsigned char)((float)g * _b);
	b = (unsigned char)((float)b * _b);
	a = (unsigned char)((float)a * _b);
	return *this;
}


RgbaColour const &RgbaColour::operator /= (float const _b)
{
	float multiplier = 1.0f / _b;
	r = (unsigned char)((float)r * multiplier);
	g = (unsigned char)((float)g * multiplier);
	b = (unsigned char)((float)b * multiplier);
	a = (unsigned char)((float)a * multiplier);
	return *this;
}


RgbaColour const &RgbaColour::operator += (RgbaColour const &_b)
{
	r += _b.r;
	g += _b.g;
	b += _b.b;
	a += _b.a;
	return *this;
}


RgbaColour const &RgbaColour::operator -= (RgbaColour const &_b)
{
	r -= _b.r;
	g -= _b.g;
	b -= _b.b;
	a -= _b.a;
	return *this;
}


bool RgbaColour::operator == (RgbaColour const &_b) const
{
	return (r == _b.r && g == _b.g && b == _b.b && a == _b.a);
}


bool RgbaColour::operator != (RgbaColour const &_b) const
{
	return (r != _b.r || g != _b.g || b != _b.b || a != _b.a);
}


unsigned char const *RgbaColour::GetData() const
{
	return &r;
}
