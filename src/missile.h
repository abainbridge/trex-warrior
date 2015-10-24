#ifndef INCLUDED_MISSILE_H
#define INCLUDED_MISSILE_H


#include "lib/vector3.h"
#include "game_obj.h"


class Missile: public GameObj
{
public:
	GameObj *m_owner;	// Missile can't hit its owner
	GameObj *m_target;
	float m_shields;

public:
	Missile(GameObj *owner, GameObj *target, Vector3 const &pos, Vector3 const &front);

	void Advance();
	void Render();

    virtual RgbaColour GetRadarColour() { return RgbaColour(255, 255, 255); }

	void Hit(float force);
};


#endif
