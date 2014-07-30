#ifndef INCLUDED_SIMPLE_PRIMITIVES_H
#define INCLUDED_SIMPLE_PRIMITIVES_H


class RgbaColour;
class Vector3;

void Render3dSprite(Vector3 const &camUp, Vector3 const &camRight,
					Vector3 const &pos, float width, float height, 
					int textureId, RgbaColour const &col);


#endif
