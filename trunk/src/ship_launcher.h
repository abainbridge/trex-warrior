#ifndef INCLUDED_SHIP_LAUNCHER_H
#define INCLUDED_SHIP_LAUNCHER_H


#include "ship.h"


class Launcher: public Ship
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
	Launcher(Vector3 const &pos);
	~Launcher();

	void Advance();

	char const *GetName() { return "launcher"; }
};


#endif
