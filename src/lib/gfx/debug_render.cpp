#include "lib/universal_include.h"
#include "debug_render.h"

#include "lib/gfx/opengl_helper.h"
#include "lib/gfx/sphere_renderer.h"
#include "lib/containers/pool_allocator.h"


struct SphereParams
{
    Vector3 pos;
    float radius;
    RgbaColour col;
};


static PoolAllocator <SphereParams> g_spheres;


void DebugRenderSphere(Vector3 const &pos, float radius, RgbaColour const &col)
{
    SphereParams *s = g_spheres.GetFree();
    s->pos = pos;
    s->radius = radius;
    s->col = col;
}


void DebugRender()
{
    GlHelperDisable(GL_LIGHTING);

    for (int i = 0; i < g_spheres.Size(); i++)
    {
        if (g_spheres.IsIndexUsed(i))
        {
            SphereParams *s = g_spheres.GetPointer(i);
            glColor3ubv(s->col.GetData());

            static Sphere sphereRenderer;
            sphereRenderer.Render(s->pos, s->radius);
            
            g_spheres.MarkNotUsed(i);
        }
    }
}
