#ifndef INCLUDED_SHIP_WASP_H
#define INCLUDED_SHIP_WASP_H


#include "ship.h"


class Wasp: public Ship
{
public:
	enum
	{
		StateShooting,
		StateMoving
	};

	float m_nextStateChangeTime;
	int m_state;
	float m_nextFireTime;

public:
	Wasp(Vector3 const &pos);
	~Wasp();

	void Advance();

	char const *GetName() { return "wasp"; }
};


#endif
