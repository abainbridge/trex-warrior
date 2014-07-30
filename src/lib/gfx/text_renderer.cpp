#include "lib/universal_include.h"
#include "lib/gfx/text_renderer.h"

#include <stdarg.h>
#include <stdio.h>

#include "lib/gfx/bitmap.h"
#include "lib/vector3.h"


TextRenderer g_gameFont;


// Horizontal size as a proportion of vertical size
#define HORIZONTAL_SIZE		0.6f

#define TEX_WIDTH			0.0666f
#define TEX_HEIGHT			0.1448f


bool TextRenderer::Initialize(char const *filename)
{
	BitmapRgba bmp;
	if (!bmp.Initialize(filename))
		return false;

	m_textureID = bmp.ConvertToTexture();

	return true;
}


void TextRenderer::BeginText2d()
{
	GLint matrixMode;
	GLint v[4];
	
	// Setup OpenGL matrices
	glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
	glGetIntegerv(GL_VIEWPORT, &v[0]);
	glMatrixMode(GL_MODELVIEW);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_modelviewMatrix);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX, m_projectionMatrix);
	glLoadIdentity();
	gluOrtho2D(v[0], v[0] + v[2], v[1] + v[3], v[1]);

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

    glEnable    (GL_BLEND);
    glDisable   (GL_CULL_FACE);
    glDisable   (GL_DEPTH_TEST);
    glDepthMask (false);

	glMatrixMode(matrixMode);
}


void TextRenderer::EndText2d()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(m_projectionMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m_modelviewMatrix);

    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}


float TextRenderer::GetTexCoordX(unsigned char c)
{
    float xIdx = (c - 32) % 14;
    float texX = xIdx * TEX_WIDTH;
    return texX + 0.002f;
}


float TextRenderer::GetTexCoordY(unsigned char c)
{
	float yIdx = (c - 32) / 14;
    float texY = 1.0f - ((0.78f + yIdx) * TEX_HEIGHT);
    return texY;
}


void TextRenderer::DrawText2dSimple(float x, float y, float size, char const *text)
{
	float horiSize = size * HORIZONTAL_SIZE;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);    

    glEnable        (GL_TEXTURE_2D);
//	glDisable(GL_BLEND);
    glBindTexture   (GL_TEXTURE_2D, m_textureID);
    
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
	for (unsigned char *c = (unsigned char *)text; *c != '\0'; c++)
    {       
		if (*c < 32)
			continue;

		float texX = GetTexCoordX(*c);
		float texY = GetTexCoordY(*c);
		float texH = TEX_HEIGHT * 0.8f;

		glBegin(GL_QUADS);
			glTexCoord2f(texX, texY + texH);		        glVertex2f(x,			 y);
			glTexCoord2f(texX + TEX_WIDTH, texY + texH);	glVertex2f(x + horiSize, y);
			glTexCoord2f(texX + TEX_WIDTH, texY);			glVertex2f(x + horiSize, y + size);
			glTexCoord2f(texX, texY);						glVertex2f(x,			 y + size);
		glEnd();

		x += horiSize;
    }

//	glEnable(GL_BLEND);
    glBlendFunc     (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable       (GL_TEXTURE_2D);
}


void TextRenderer::DrawText2d(float _x, float _y, float _size, char const *_text, ...)
{
    char buf[512];
    va_list ap;
    va_start (ap, _text);
    vsprintf(buf, _text, ap);
    DrawText2dSimple(_x, _y, _size, buf);
}


void TextRenderer::DrawText2dCentre(float _x, float _y, float _size, char const *_text, ...)
{
    char buf[512];
    va_list ap;
    va_start (ap, _text);
    vsprintf(buf, _text, ap);

    float width = GetTextWidth(strlen(buf), _size);
    DrawText2dSimple(_x - width/2, _y, _size, buf);
}


float TextRenderer::GetTextWidth(unsigned int _numChars, float _size)
{
    return _numChars * _size * HORIZONTAL_SIZE;
}
