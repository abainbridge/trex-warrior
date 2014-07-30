#ifndef INCLUDED_ARENA_H
#define INCLUDED_ARENA_H


#include "lib/vector3.h"
#include "game_obj.h"


class Shape;


class Arena: public GameObj
{
public:
	Shape *m_floor;		// Separate from the perimeter walls so that we can avoid doing hit-check on it.

	Arena(Vector3 pos, Vector3 front);

	bool IsInside(Vector3 pos);	// Returns true if the specified vector is within the bounds of the arena
	void Render();
};


#endif
