#ifndef INCLUDED_BUILDING_H
#define INCLUDED_BUILDING_H


#include "lib/matrix33.h"
#include "lib/vector3.h"
#include "game_obj.h"


class Shape;


class Building: public GameObj
{
public:
    Matrix33 m_orientation;
    Vector3 m_angVel;
    Vector3 m_momentOfInertia;

	Building(Vector3 pos, Vector3 front);

    void Advance();
    void Render();

    virtual RgbaColour GetRadarColour() { return RgbaColour(0, 255, 0); }
};


#endif
