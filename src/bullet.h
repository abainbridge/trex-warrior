#ifndef INCLUDED_BULLET_H
#define INCLUDED_BULLET_H


#include "lib/vector3.h"
#include "game_obj.h"


float const BULLET_SIZE	= 9.0f;


class Bullet: public GameObj
{
public:
	Vector3 m_vel;
	GameObj *m_owner;	// Bullet can't hit its owner

public:
	Bullet(GameObj *owner, Vector3 const &pos, Vector3 const &front);

	void Advance();
	void Render();

    virtual RgbaColour GetRadarColour() { return RgbaColour(255, 190, 0); }
};


#endif
