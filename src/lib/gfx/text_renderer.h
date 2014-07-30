#ifndef INCLUDED_TEXT_RENDERER_H
#define INCLUDED_TEXT_RENDERER_H

class RgbaColour;
class Vector3;


#define DEF_FONT_SIZE 12.0f


class TextRenderer
{
protected:
	double			m_projectionMatrix[16];
	double			m_modelviewMatrix[16];
	unsigned int    m_textureID;

    float GetTexCoordX   (unsigned char theChar);
    float GetTexCoordY   (unsigned char theChar);
    
public:
	bool Initialize		 (char const *_filename);

    void BeginText2d	 ();
    void EndText2d		 ();
    
    void DrawText2dSimple(float x, float y, float size, char const *text);
    void DrawText2d      (float x, float y, float size, char const *text, ...);	// Like simple but with variable args
    void DrawText2dCentre(float x, float y, float size, char const *text, ...);	// Like above but with centre justify
	
    float GetTextWidth   (unsigned int numChars, float _size=13.0f);
};


extern TextRenderer g_gameFont;


#endif
