#ifndef INCLUDED_SHIP_SPEEDY_H
#define INCLUDED_SHIP_SPEEDY_H


#include "ship.h"


class Speedy: public Ship
{
public:
	enum
	{
		StateShooting,
		StateMoving
	};

	NavRouteFollower *m_navRouteFollower;
	float m_nextStateChangeTime;
	int m_state;
	float m_nextFireTime;

public:
	Speedy(Vector3 const &pos);
	~Speedy();

	void Advance();

	char const *GetName() { return "speedy"; }
};


#endif
