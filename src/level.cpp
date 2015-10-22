#include "lib/universal_include.h"
#include "level.h"

#include "lib/gfx/shape.h"
#include "lib/resource.h"

#include "arena.h"
#include "building.h"
#include "jump_pad.h"
#include "navigation.h"
#include "particle_system.h"
#include "script.h"
#include "ship_player.h"
#include "ship_speedy.h"


Level *g_level = NULL;


Level::Level()
{
	m_arena = new Arena(g_zeroVector, Vector3(0,0,1));
	m_objects.PushBack(m_arena);

	m_playerShip = new ShipPlayer(Vector3(-200,PLAYER_SHIP_HOVER_HEIGHT,50));
	m_playerShip->m_front.Set(1,0,0);
	m_objects.PushBack(m_playerShip);

	Building *building = new Building(Vector3(-20, 0, 230), Vector3(0,0,1));
	m_objects.PushBack(building);

	building = new Building(Vector3(-20, 0, -140), Vector3(0,0,1));
	m_objects.PushBack(building);
	
	building = new Building(Vector3(-110, 0, -250), Vector3(1,0,0));
	m_objects.PushBack(building);

	building = new Building(Vector3(70, 0, -250), Vector3(1,0,0));
	m_objects.PushBack(building);

	JumpPad *jumpPad = new JumpPad(Vector3(0, 0, 0));
	m_objects.PushBack(jumpPad);

	m_navRoutes.PushBack(new NavRoute("level1_speedy"));

	m_script = new Script("data/scripts/level1.txt");

	g_level = this;
}


Level::~Level()
{
	m_objects.ClearAndDelete();
	g_level = NULL;
}


void Level::Advance()
{
	int numObjs = m_objects.Size();
	for (int i = 0; i < numObjs; i++)
		m_objects[i]->Advance();

	// Remove objects listed for delete
	numObjs = m_objectsToDelete.Size();
	for (int i = 0; i < numObjs; i++)
	{
		GameObj *o = m_objectsToDelete.RemoveDataAtEnd();
		int idx = m_objects.FindData(o);
		if (idx >= 0)
			m_objects.RemoveData(idx);
		delete o;
	}

	g_particleSystem.Advance();
	m_script->Advance();
}


void Level::Render()
{
	int numObjs = m_objects.Size();
	for (int i = 0; i < numObjs; i++)
		m_objects[i]->Render();

	g_particleSystem.Render();
	g_level->m_script->Render3d();
}


void Level::DeleteObj(GameObj *obj)
{
	int idx = m_objectsToDelete.FindData(obj);
	if (idx < 0)
		m_objectsToDelete.PushBack(obj);
}


bool Level::AreEnemiesAlive()
{
	for (int i = 0; i < m_objects.Size(); i++)
	{
		switch (m_objects[i]->m_type)
		{
		case GameObj::ObjTypeSpeedy:
		case GameObj::ObjTypeWasp:
		case GameObj::ObjTypeLauncher:
			return true;
		}
	}

	return false;
}