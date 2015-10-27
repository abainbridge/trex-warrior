#include "lib/universal_include.h"
#include "simple_primitives.h"

#include "lib/gfx/opengl_helper.h"
#include "lib/gfx/rgb_colour.h"
#include "lib/vector3.h"


void Render3dSprite(Vector3 const &camUp, Vector3 const &camRight,
					Vector3 const &pos, float width, float height, 
					int textureId, RgbaColour const &col)
{
	Vector3 right = camRight * (width * 0.5f);
	Vector3 up = -camUp * (height * 0.5f);

	Vector3 bottomLeft(pos - right - up);
	Vector3 bottomRight(pos + right - up);
	Vector3 topLeft(pos - right + up);
	Vector3 topRight(pos + right + up);

	GlHelperDisable(GL_LIGHTING);
	GlHelperEnable(GL_TEXTURE_2D);
    GlHelperDisable(GL_CULL_FACE);
	glDepthMask(false);
    GlHelperEnable(GL_BLEND);

	glColor4ubv(col.GetData());
	glBindTexture(GL_TEXTURE_2D, textureId);
	glBegin(GL_QUADS);
		glTexCoord2f(1, 1);
		glVertex3fv(topLeft.GetData());
		glTexCoord2f(0, 1);
		glVertex3fv(topRight.GetData());
		glTexCoord2f(0, 0);
		glVertex3fv(bottomRight.GetData());
		glTexCoord2f(1, 0);
		glVertex3fv(bottomLeft.GetData());
	glEnd();

    glDepthMask(true);
}
