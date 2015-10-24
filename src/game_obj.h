#ifndef INCLUDE_GAME_OBJ_H
#define INCLUDE_GAME_OBJ_H


#include "lib/vector3.h"


class Shape;


class GameObj
{
public:
    enum Type
    {
		ObjTypeArena,
		ObjTypePlayerShip,
		ObjTypeSpeedy,
		ObjTypeWasp,
		ObjTypeLauncher,
		ObjTypeBullet,
		ObjTypeMissile,
		ObjTypeBuilding,
		ObjTypeJumpPad
    };
    
    int m_type;
	Vector3 m_pos;
	Vector3 m_front;
	Shape *m_shape;

public:
    GameObj(int type, Vector3 pos) 
	{ 
		m_type = type; 
		m_pos = pos;
		m_front = Vector3(1, 0, 0);
		m_shape = NULL;
	}

	virtual void Advance() {};
    virtual void Render() = 0;

	virtual char const *GetName() { DebugAssert(0); return "¬"; }
	virtual void TakeHit(float force) {}

	// Properties to consider adding
	// 1. bool BulletCausesDamage
	// 2. bool IsEnemy
};


#endif
