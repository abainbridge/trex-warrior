// This module implements a system to simplify rendering shapes for debug. The
// typical problem it solves it when you want to render something based on 
// some state that is available in the advance phase of the game loop but is
// not available in the render phase. This module provides temporary storage
// for this state that will be reclaimed after it is used in the render phase.

#ifndef INCLUDED_DEBUG_RENDER_H
#define INCLUDED_DEBUG_RENDER_H

#include "lib/gfx/rgb_colour.h"
#include "lib/vector3.h"


void DebugRenderSphere(Vector3 const &centre, float radius, RgbaColour const &col = g_colourWhite);
void DebugRenderLine(Vector3 const &start, Vector3 const &end, RgbaColour const &col = g_colourWhite);

void DebugRender();
void DebugRenderClear();

#endif
