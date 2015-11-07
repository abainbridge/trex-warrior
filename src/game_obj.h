#ifndef INCLUDE_GAME_OBJ_H
#define INCLUDE_GAME_OBJ_H


#include "lib/gfx/rgb_colour.h"
#include "lib/vector3.h"


class Shape;


class GameObj
{
public:
    enum Type
    {
		ObjTypeArena = 1,
		ObjTypePlayerShip = 2,
		ObjTypeSpeedy = 4,
		ObjTypeWasp = 8,
		ObjTypeLauncher = 16,
		ObjTypeBullet = 32,
		ObjTypeMissile = 64,
		ObjTypeBuilding = 128,
		ObjTypeJumpPad = 256,
        ObjTypeBomb = 512,
        ObjTypeShieldPowerup = 1024
    };
    
    int m_type;
	Vector3 m_pos;
	Vector3 m_front;
	Shape *m_shape;

public:
    GameObj(int type, Vector3 const &pos) 
	{ 
		m_type = type; 
		m_pos = pos;
		m_front.Set(1, 0, 0);
		m_shape = NULL;
	}

    Vector3 GetRight() { return m_front.CrossProduct(g_upVector); }

	virtual void Advance() {};
    virtual void Render();

	virtual char const *GetName() { DebugAssert(0); return "¬"; }
	virtual void TakeHit(float force) {}

    virtual RgbaColour GetRadarColour() = 0;

	// Properties to consider adding
	// 1. bool BulletCausesDamage
	// 2. bool IsEnemy
};


const int OBJTYPE_MASK_ALL_DAMAGABLE = GameObj::ObjTypeBomb |
                                       GameObj::ObjTypeLauncher |
                                       GameObj::ObjTypeMissile |
                                       GameObj::ObjTypePlayerShip |
                                       GameObj::ObjTypeSpeedy |
                                       GameObj::ObjTypeWasp;


#endif
