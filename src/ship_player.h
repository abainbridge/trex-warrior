#ifndef INCLUDED_SHIP_PLAYER_H
#define INCLUDED_SHIP_PLAYER_H


#include "lib/vector3.h"
#include "ship.h"


#define PLAYER_SHIP_MAX_MAX_SPEED (270.0f)
#define PLAYER_SHIP_MAX_SHIELDS 18.0f
#define PLAYER_SHIP_HOVER_HEIGHT 4.0f


class ShipPlayer: public Ship
{
public:
	float m_laserTemp;
	float m_maxSpeed;
	Vector3 m_whackVel;	// Velocity caused do to the impulse of a collision or by a JumpPad
	float m_rotateVel;	// Velocity of the ships rotation (around the vertical axis)

public:
	ShipPlayer(Vector3 const &pos);
	~ShipPlayer();

	void Advance();
};


#endif
