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


struct LineParams
{
    Vector3 start, end;
    RgbaColour col;
};


static PoolAllocator <SphereParams> g_spheres;
static PoolAllocator <LineParams> g_lines;


void DebugRenderSphere(Vector3 const &pos, float radius, RgbaColour const &col)
{
    SphereParams *s = g_spheres.GetFree();
    s->pos = pos;
    s->radius = radius;
    s->col = col;
}


void DebugRenderLine(Vector3 const &start, Vector3 const &end, RgbaColour const &col /* = g_colourWhite */)
{
    LineParams *l = g_lines.GetFree();
    l->start = start;
    l->end = end;
    l->col = col;
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
        }
    }

    glLineWidth(1.0f);

    for (int i = 0; i < g_lines.Size(); i++)
    {
        if (g_lines.IsIndexUsed(i))
        {
            LineParams *l = g_lines.GetPointer(i);
            glColor3ubv(l->col.GetData());

            glBegin(GL_LINES);
            glVertex3fv(l->start.GetData());
            glVertex3fv(l->end.GetData());
            glEnd();
        }
    }
}


void DebugRenderClear()
{
    for (int i = 0; i < g_spheres.Size(); i++)
        g_spheres.MarkNotUsed(i);
    for (int i = 0; i < g_lines.Size(); i++)
        g_lines.MarkNotUsed(i);
}
