#ifndef INCLUDE_NAVIGATION_H
#define INCLUDE_NAVIGATION_H


#include "lib/containers/list.h"
#include "lib/lite_string.h"
#include "lib/vector2.h"


struct NavRouteFollower
{
	Vector2 pos;
	Vector2 front;
	float maxSpeed;
	int targetWayPointIdx;
};


class NavRoute
{
public:
    List <Vector2 *> m_wayPoints;
	LiteString m_name;

public:
	NavRoute(char const *name);
	~NavRoute();

	bool Load();
	bool Save();

	void NextPos(NavRouteFollower *nrf);
};


#endif
