#ifndef INCLUDED_DEBUG_RENDER_H
#define INCLUDED_DEBUG_RENDER_H

#include "lib/gfx/rgb_colour.h"
#include "lib/vector3.h"


void DebugRenderSphere(Vector3 const &centre, float radius, RgbaColour const &col = g_colourWhite);
void DebugRenderLine(Vector3 const &start, Vector3 const &end, RgbaColour const &col = g_colourWhite);

void DebugRender();
void DebugRenderClear();

#endif
