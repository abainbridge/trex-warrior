#ifndef INCLUDED_ENEMY_SHIP_H
#define INCLUDED_ENEMY_SHIP_H


#include "lib/vector3.h"
#include "game_obj.h"


struct NavRouteFollower;


class Ship: public GameObj
{
public:
	float m_shields;
	float m_speed;

public:
	Ship(int type, Vector3 const &pos);

	virtual void TakeHit(float force);

    virtual RgbaColour GetRadarColour() { return RgbaColour(0, 255, 255); }

	void Render();
};


#endif
