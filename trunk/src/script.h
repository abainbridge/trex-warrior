#ifndef INCLUDED_SCRIPT_H
#define INCLUDED_SCRIPT_H


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
	
	float m_textEndTime1;
	float m_textEndTime2;
	LiteString m_text1;
	LiteString m_text2;

	Script(char const *filename);

	void Advance();
	void Render2d();
	void Render3d();

	void ShowText(char const *text);
};


#endif
