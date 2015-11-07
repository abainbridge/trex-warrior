#ifndef INCLUDED_SHIP_SPEEDY_H
#define INCLUDED_SHIP_SPEEDY_H


#include "ship.h"


class SpherePackage;


class Speedy: public Ship
{
public:
	enum
	{
		StateShooting,
		StateMoving,
        StateAvoidingCorner
	};

    float m_avoidCornerSteeringTorque;
	float m_nextStateChangeTime;
	int m_state;
	float m_nextFireTime;

private:
    void LeftAndRightRayHits(bool *leftHit, Vector3 *leftHitPos, bool *rightHit, Vector3 *rightHitPos);

    void DoShooting();
    void DoMoving();
    void DoAvoidingCorner();

public:
	Speedy(Vector3 const &pos);
	~Speedy();

	void Advance();

	char const *GetName() { return "speedy"; }
};


#endif
