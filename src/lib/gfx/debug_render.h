#ifndef INCLUDED_DEBUG_RENDER_H
#define INCLUDED_DEBUG_RENDER_H

#include "lib/gfx/rgb_colour.h"
#include "lib/vector3.h"

void RenderSphere(Vector3 const &centre, float radius, RgbaColour const &col=RgbaColour(255,255,255));

#endif