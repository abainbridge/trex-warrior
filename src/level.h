#ifndef INCLUDED_LEVEL_H
#define INCLUDED_LEVEL_H


#include "lib/containers/list.h"


class Arena;
class GameObj;
class NavRoute;
class RayPackage;
class Script;
class ShipPlayer;
class Shape;
class SpherePackage;
class Vector3;


class Level
{
private:
	List <GameObj *> m_objectsToDelete;	// Items on here will be deleted at the end of the location advance

public:
	ShipPlayer *m_playerShip;
	Arena *m_arena;
	List <GameObj *> m_objects;
	List <NavRoute *> m_navRoutes;
	Script *m_script;

public:
	Level();
	~Level();

	void Advance();
	void Render();

	void AddObj(GameObj *obj) { m_objects.PushBack(obj); }
	void DeleteObj(GameObj *obj);

    void SpawnEnemy(int objType, Vector3 const *pos);

	bool AreEnemiesAlive();	// Returns true if there are currently any enemies alive

    GameObj *RayHit(RayPackage *ray, int gameObjTypesMask, GameObj *objToIgnore, Vector3 *hitPos);
    GameObj *SphereHit(SpherePackage *sphere, int gameObjTypesMask, GameObj *objToIgnore, Vector3 *hitPos);
};


extern Level *g_level;


#endif
