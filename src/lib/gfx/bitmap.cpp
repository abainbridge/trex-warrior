#include "lib/universal_include.h"
#include "lib/gfx/bitmap.h"

#include "lib/gfx/rgb_colour.h"
#include "lib/binary_file_reader.h"
#include "lib/debug_utils.h"
#include "lib/filesys_utils.h"


#define BMP_RGB				0
#define OS2INFOHEADERSIZE  12
#define WININFOHEADERSIZE  40


class BitmapFileHeader
{
public:
	unsigned short bfType;
	unsigned long  bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long  bfOffBits;
};


// Used for both OS/2 and Windows BMP. 
// Contains only the parameters needed to load the image 
class BitmapInfoHeader
{
public:
	unsigned long  biWidth;
	unsigned long  biHeight;
	unsigned short biBitCount;
	unsigned long  biCompression;
};


class WinBmpInfoHeader
{
public:
	unsigned long  biWidth;
	unsigned long  biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long  biCompression;
	unsigned long  biSizeImage;
	unsigned long  biXPelsPerMeter;
	unsigned long  biYPelsPerMeter;
	unsigned long  biClrUsed;
	unsigned long  biClrImportant;
};


class OS2BmpInfoHeader
{
public:
	unsigned short biWidth;
	unsigned short biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
};



// ****************************************************************************
// Private Functions
// ****************************************************************************

// Reads a BMP file header and check that it has the BMP magic number.
void BitmapRgba::ReadBmpFileHeader(BinaryFileReader *f, BitmapFileHeader *fileheader)
{
	fileheader->bfType = f->ReadS16();
	fileheader->bfSize= f->ReadS32();
	fileheader->bfReserved1= f->ReadS16();
	fileheader->bfReserved2= f->ReadS16();
	fileheader->bfOffBits= f->ReadS32();

	DebugAssert(fileheader->bfType == 19778);
}


// Reads information from a BMP file header.
void BitmapRgba::ReadWinBmpInfoHeader(BinaryFileReader *f, BitmapInfoHeader *infoheader)
{
	WinBmpInfoHeader win_infoheader;

	win_infoheader.biWidth = f->ReadS32();
	win_infoheader.biHeight = f->ReadS32();
	win_infoheader.biPlanes = f->ReadS16();
	win_infoheader.biBitCount = f->ReadS16();
	win_infoheader.biCompression = f->ReadS32();
	win_infoheader.biSizeImage = f->ReadS32();
	win_infoheader.biXPelsPerMeter = f->ReadS32();
	win_infoheader.biYPelsPerMeter = f->ReadS32();
	win_infoheader.biClrUsed = f->ReadS32();
	win_infoheader.biClrImportant = f->ReadS32();

	infoheader->biWidth = win_infoheader.biWidth;
	infoheader->biHeight = win_infoheader.biHeight;
	infoheader->biBitCount = win_infoheader.biBitCount;
	infoheader->biCompression = win_infoheader.biCompression;
}


// Reads information from an OS/2 format BMP file header.
void BitmapRgba::ReadOs2BmpInfoHeader(BinaryFileReader *f, BitmapInfoHeader *infoheader)
{
	OS2BmpInfoHeader os2_infoheader;

	os2_infoheader.biWidth = f->ReadS16();
	os2_infoheader.biHeight = f->ReadS16();
	os2_infoheader.biPlanes = f->ReadS16();
	os2_infoheader.biBitCount = f->ReadS16();

	infoheader->biWidth = os2_infoheader.biWidth;
	infoheader->biHeight = os2_infoheader.biHeight;
	infoheader->biBitCount = os2_infoheader.biBitCount;
	infoheader->biCompression = 0;
}


void BitmapRgba::ReadBmpPalette(int ncols, RgbaColour pal[256], BinaryFileReader *f, int win_flag)
{
	for (int i = 0; i < ncols; i++) 
	{
	    pal[i].b = f->ReadU8();
	    pal[i].g = f->ReadU8();
	    pal[i].r = f->ReadU8();
		pal[i].a = 255;
	    if (win_flag) f->ReadU8();
	}
}


// Support function for reading the 4 bit bitmap file format.
void BitmapRgba::Read4BitLine(int length, BinaryFileReader *f, RgbaColour pal[256], int y)
{
	for (int x = 0; x < length; x += 2) 
	{
		unsigned char i = f->ReadU8();
		unsigned idx1 = i & 15;
		unsigned idx2 = (i >> 4) & 15;
		PutPixel(x+1, y, pal[idx1]);
		PutPixel(x, y, pal[idx2]);
	}
}


// Support function for reading the 8 bit bitmap file format.
void BitmapRgba::Read8BitLine(int length, BinaryFileReader *f, RgbaColour pal[256], int y)
{
//	y = m_height - line - 1;
	for (int x = 0; x < length; ++x) 
	{
		unsigned char i = f->ReadU8();
		PutPixel(x, y, pal[i]);
	}
}


// Support function for reading the 24 bit bitmap file format
void BitmapRgba::Read24BitLine(int length, BinaryFileReader *f, int y)
{
	RgbaColour c;
	int nbytes=0;
//	y = m_height - y - 1;
	c.a = 255;

	for (int i=0; i<length; i++) 
	{
		c.b = f->ReadU8();
		c.g = f->ReadU8();
		c.r = f->ReadU8();
		PutPixel(i, y, c);
		nbytes += 3;
	}

	for (int padding = (4 - nbytes) & 3; padding; --padding) 
		f->ReadU8();
}


void BitmapRgba::LoadBmp(BinaryFileReader *_in)
{
	BitmapFileHeader fileheader;
	BitmapInfoHeader infoheader;
	RgbaColour palette[256];

	ReadBmpFileHeader(_in, &fileheader);

	unsigned long biSize = _in->ReadS32();

	if (biSize == WININFOHEADERSIZE) 
	{
		ReadWinBmpInfoHeader(_in, &infoheader);
		
		int ncol = (fileheader.bfOffBits - 54) / 4; // compute number of colors recorded
		ReadBmpPalette(ncol, palette, _in, 1);
	}
	else if (biSize == OS2INFOHEADERSIZE) 
	{
	    ReadOs2BmpInfoHeader(_in, &infoheader);
    
	    int ncol = (fileheader.bfOffBits - 26) / 3; // compute number of colors recorded
	    ReadBmpPalette(ncol, palette, _in, 0);
	}
	else 
	{
	    DebugAssert(0);
	}

	Initialize(infoheader.biWidth, infoheader.biHeight);
	DebugAssert(infoheader.biCompression == BMP_RGB); 
	DebugAssert(!_in->IsEof());

	// Read the image
	for (int i = 0; i < (int)infoheader.biHeight; ++i) 
	{
		switch (infoheader.biBitCount)
		{
		case 4:
			Read4BitLine(infoheader.biWidth, _in, palette, i);
			break;
		case 8:
			Read8BitLine(infoheader.biWidth, _in, palette, i);
			break;
		case 24:
			Read24BitLine(infoheader.biWidth, _in, i);
			break;
		default:
			DebugAssert(0);
			break;
		}
	}
}


// ****************************************************************************
// Public Functions
// ****************************************************************************

BitmapRgba::BitmapRgba()
:	m_width(-1),
	m_height(-1),
	m_pixels(NULL),
	m_lines(NULL)
{
}


BitmapRgba::~BitmapRgba()
{
	m_width = -1;
	m_height = -1;
	delete [] m_pixels;
	delete [] m_lines;
	m_pixels = NULL;
	m_lines = NULL;
}


void BitmapRgba::Initialize(int width, int height)
{
	m_width = width;
	m_height = height;
	m_pixels = new RgbaColour[width * height];
	m_lines = new RgbaColour *[height];

	DebugAssert(m_pixels && m_lines);

	for (int y = 0; y < height; ++y)
		m_lines[y] = &m_pixels[y * width];
}
	

bool BitmapRgba::Initialize(char const *filename)
{
	BinaryFileReader in;
	if (!in.Open(filename))
		return false;

	char const *extension = GetExtensionPart(filename);
	DebugAssert(stricmp(extension, "bmp") == 0);
	LoadBmp(&in);

	return true;
}


void BitmapRgba::PutPixel(int x, int y, RgbaColour const &colour)
{
	m_lines[y][x] = colour;
}


RgbaColour const &BitmapRgba::GetPixel(int x, int y) const
{
	return m_lines[y][x];
}


void BitmapRgba::ConvertPinkToTransparent() 
{
	const RgbaColour pink(255, 0, 255);
	const RgbaColour trans(128,128,128,0);

	for (int y = 0; y < m_height; ++y) 
	{
		for (int x = 0; x < m_width; ++x) 
		{
			RgbaColour const c(GetPixel(x, y));
			if (c == pink)
			{
				PutPixel(x, y, trans);
			}
		}
	}
}


int BitmapRgba::ConvertToTexture(bool mipmapping) const
{
	unsigned int texId;

    glEnable        (GL_TEXTURE_2D);
	glGenTextures	(1, &texId);
	glBindTexture	(GL_TEXTURE_2D, texId);
    glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	
//    bool texturingWasEnabled = glIsEnabled(GL_TEXTURE_2D);

	glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	
    if (mipmapping)
	{
		int result = gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 
					                   m_width, m_height, 
						               GL_RGBA, GL_UNSIGNED_BYTE, 
						               m_pixels);
        
        ReleaseAssert( result == 0, "ConvertToTexture failed with error : %s", gluErrorString(result) );
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 4, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels);
	}

//    if (!texturingWasEnabled)
//    {
//        glDisable(GL_TEXTURE_2D);
//    }

	return (int) texId;
}
