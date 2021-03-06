#include "lib/universal_include.h"
#include "script.h"

#include "lib/gfx/debug_render.h"
#include "lib/gfx/text_renderer.h"
#include "lib/hi_res_time.h"
#include "lib/text_file_reader.h"
#include "lib/window_manager.h"
#include "app.h"
#include "game_obj.h"
#include "hud_text.h"
#include "level.h"


Script::Script(char const *filename)
{
	m_currentItem = -1;

	m_spawnPoints.PushBack(new Vector2(-200, 100));
	m_spawnPoints.PushBack(new Vector2(200, 100));
	m_spawnPoints.PushBack(new Vector2(0, -240));

	TextFileReader in(filename);
	ReleaseAssert(in.IsOpen(), "Couldn't open file '%s'", filename);

	while (in.ReadLine())
	{
		char const *tok = in.GetNextToken();
		if (!tok)
			continue;

		ScriptItem *si = new ScriptItem;
		if (stricmp(tok, "text") == 0)
		{
			si->m_type = ScriptItem::CommandText;
			si->m_text = in.GetRestOfLine();
		}
		else if (stricmp(tok, "wait") == 0)
		{
			si->m_type = ScriptItem::CommandWait;
			char const *untilStr = in.GetNextToken();
			ReleaseAssert(untilStr, "Wait command corrupt on line %d", in.m_lineNum);
			if (stricmp(untilStr, "death") == 0)
				si->m_waitUntil = -1;
			else 
				si->m_waitUntil = atoi(untilStr);
		}
		else if (stricmp(tok, "spawn") == 0)
		{
			si->m_type = ScriptItem::CommandSpawn;
			char const *objType = in.GetNextToken();
			char const *spawnPointStr = in.GetNextToken();

			ReleaseAssert(objType && spawnPointStr, "Spawn command corrupt on line %d", in.m_lineNum);

			int spawnPointIdx = atoi(spawnPointStr);
			ReleaseAssert(m_spawnPoints.Size() > spawnPointIdx, 
				"Spawn command corrupt on line %d", in.m_lineNum);
			
			if (stricmp(objType, "Speedy") == 0)
				si->m_objTypeToSpawn = GameObj::ObjTypeSpeedy;
			else if (stricmp(objType, "Wasp") == 0)
				si->m_objTypeToSpawn = GameObj::ObjTypeWasp;
			else if (stricmp(objType, "Launcher") == 0)
				si->m_objTypeToSpawn = GameObj::ObjTypeLauncher;
 			else
 				ReleaseAssert(0, "Spawn command corrupt on line %d", in.m_lineNum);

			si->m_spawnPoint = spawnPointIdx;
		}
	
		m_items.PushBack(si);
	}
}


void Script::BeginNextCommand()
{
	m_currentItem++;
	if (m_currentItem >= m_items.Size())
		return;

	ScriptItem *si = m_items[m_currentItem];

	while (si)
	{
		switch (si->m_type)
		{
		case ScriptItem::CommandText:
			g_hudText.ShowText(si->m_text.m_str);
			break;
		case ScriptItem::CommandWait:
			if (si->m_waitUntil > 0)
				m_waitEndTime = g_gameTime + si->m_waitUntil;

			return;
			break;
		case ScriptItem::CommandSpawn:
			Vector3 pos = *(m_spawnPoints[si->m_spawnPoint]);
			g_level->SpawnEnemy(si->m_objTypeToSpawn, &pos);
			break;
		}

		m_currentItem++;
		si = m_items[m_currentItem];
	}
}


void Script::Advance()
{
	if (m_currentItem == -1)
		BeginNextCommand();

	if (m_currentItem >= m_items.Size())
		return;

	ScriptItem *si = m_items[m_currentItem];

	if (si->m_type == ScriptItem::CommandWait)
	{
		if (si->m_waitUntil <= 0)
		{
			if (!g_level->AreEnemiesAlive())
				BeginNextCommand();
		}
		else if (g_gameTime > m_waitEndTime)
		{
			BeginNextCommand();
		}
	}
}


void Script::Render3d()
{
// 	Vector3 c(*(m_spawnPoints[0]));
// 	RenderSphere(c, 5);
}


