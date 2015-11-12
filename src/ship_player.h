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

public:
	ShipPlayer(Vector3 const &pos);
	~ShipPlayer();

	void Advance();

    void IncreaseShields();
};


#endif
