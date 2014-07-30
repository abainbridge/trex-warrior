#ifndef INCLUDED_BITMAP_H
#define INCLUDED_BITMAP_H


class RgbaColour;
class BitmapFileHeader;
class BitmapInfoHeader;
class BinaryFileReader;


class BitmapRgba
{
private:
	void ReadBmpFileHeader(BinaryFileReader *f, BitmapFileHeader *fileheader);
	void ReadWinBmpInfoHeader(BinaryFileReader *f, BitmapInfoHeader *infoheader);
	void ReadOs2BmpInfoHeader(BinaryFileReader *f, BitmapInfoHeader *infoheader);

	void ReadBmpPalette(int ncols, RgbaColour pal[256], BinaryFileReader *f, int winFlag);
	void Read4BitLine(int length, BinaryFileReader *f, RgbaColour *pal, int line);
	void Read8BitLine(int length, BinaryFileReader *f, RgbaColour *pal, int line);
	void Read24BitLine(int length, BinaryFileReader *f, int line);
	void LoadBmp(BinaryFileReader *in);
	
public:
	int m_width;
	int m_height;
	RgbaColour *m_pixels;
	RgbaColour **m_lines;

	BitmapRgba();
	~BitmapRgba();

	void Initialize(int width, int height);
	bool Initialize(char const *filename);

	void PutPixel(int x, int y, RgbaColour const &colour);
	RgbaColour const &GetPixel(int x, int y) const;

	void ConvertPinkToTransparent();
	int ConvertToTexture(bool mipmapping = true) const;
};


#endif
