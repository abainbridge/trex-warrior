#ifndef INCLUDED_SCRIPT_H
#define INCLUDED_SCRIPT_H


// This script module implements the behaviour of a level in the game. It loads
// a script .txt file and executes the commands from it one at a time. When the
// end of the script is reached, the level is complete.


#include "lib/containers/list.h"
#include "lib/lite_string.h"
#include "lib/vector2.h"


struct ScriptItem
{
	enum
	{
		CommandText,
		CommandWait,
		CommandSpawn
	};

	int m_type;

	LiteString m_text;

	int m_waitUntil;	// 0 = all enemies dead, +ve means number of seconds to wait

	int m_objTypeToSpawn;
	int m_spawnPoint;
};


class Script
{
private:
	void BeginNextCommand();

public:
	List<Vector2 *> m_spawnPoints;

	List <ScriptItem *> m_items;
	int m_currentItem;
	
	float m_waitEndTime;
	
	Script(char const *filename);

	void Advance();
	void Render3d();
};


#endif
