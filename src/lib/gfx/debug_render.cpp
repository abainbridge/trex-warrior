#include "lib/universal_include.h"
#include "debug_render.h"

#include "lib/gfx/sphere_renderer.h"


void RenderSphere(Vector3 const &centre, float radius, RgbaColour const &col)
{
	static Sphere aSphere;

	glDisable(GL_LIGHTING);
	glColor3ubv(col.GetData());
	aSphere.Render(centre, radius);
	glEnable(GL_LIGHTING);
}
