#ifndef INCLUDED_LEVEL_H
#define INCLUDED_LEVEL_H


#include "lib/containers/list.h"


class Arena;
class GameObj;
class NavRoute;
class Script;
class ShipPlayer;
class Shape;


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

	bool AreEnemiesAlive();	// Returns true if there are currently any enemies alive
};


extern Level *g_level;


#endif
