#ifndef INCLUDED_RGB_COLOUR_H
#define INCLUDED_RGB_COLOUR_H


class RgbaColour
{
public:
	unsigned char r, g, b, a;

	RgbaColour() {}
	RgbaColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
    RgbaColour(int _col);

	void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
    void Set(int _col);

	RgbaColour operator + (RgbaColour const &b) const;
	RgbaColour operator - (RgbaColour const &b) const;
	RgbaColour operator * (float const b) const;
	RgbaColour operator / (float const b) const;

    RgbaColour const &operator = (RgbaColour const &b);
	RgbaColour const &operator *= (float const b);
	RgbaColour const &operator /= (float const b);
	RgbaColour const &operator += (RgbaColour const &b);
	RgbaColour const &operator -= (RgbaColour const &b);

	bool operator == (RgbaColour const &b) const;
	bool operator != (RgbaColour const &b) const;

	unsigned char const *GetData() const;
};

// Operator * between float and RGBAColour
inline RgbaColour operator * (float scale, RgbaColour const &v)
{
	return v * scale;
}


extern RgbaColour g_colourBlack;
extern RgbaColour g_colourWhite;


#endif
