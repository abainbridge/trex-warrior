#ifndef INCLUDED_ENEMY_SHIP_H
#define INCLUDED_ENEMY_SHIP_H


#include "lib/vector3.h"
#include "game_obj.h"


class Ship: public GameObj
{
public:
	float m_shields;
	float m_speed;
    float m_targetSpeed;
    Vector3 m_whackVel;	// Velocity caused do to the impulse of a collision or by a JumpPad
    float m_rotateVel;	// Velocity of the ships rotation (around the vertical axis)

public:
	Ship(int type, Vector3 const &pos);

	virtual void TakeHit(float force);

    virtual RgbaColour GetRadarColour() { return RgbaColour(0, 255, 255); }

    virtual void Advance();
	virtual void Render();
};


#endif
