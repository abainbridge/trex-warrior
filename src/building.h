#ifndef INCLUDED_BUILDING_H
#define INCLUDED_BUILDING_H


#include "lib/vector3.h"
#include "game_obj.h"


class Shape;


class Building: public GameObj
{
public:
	Building(Vector3 pos, Vector3 front);

	void Render();
};


#endif
