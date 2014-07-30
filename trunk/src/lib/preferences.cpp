#include "lib/universal_include.h"
#include "lib/preferences.h"

#include <stdio.h>
#include <string.h>

#include "lib/debug_utils.h"
#include "lib/text_file_reader.h"


PrefsManager g_prefsManager;


PrefsItem *PrefsManager::GetItem(char const *key)
{
	for (int i = 0; i < m_items.Size(); i++)
	{
		PrefsItem *item = m_items[i];
		if (stricmp(key, item->m_key) == 0)
			return item;
	}  

	return NULL;
}


void PrefsManager::RegisterInt(char const *key, int val)
{
	DebugAssert(!GetItem(key));
	m_items.PushBack(new PrefsItem(key, val));
}


bool PrefsManager::Load(char const *filename)
{
    m_filename = filename;

	TextFileReader f(filename);

	if (!f.IsOpen())
		return false;

	while (f.ReadLine())
	{
		if (!f.TokenAvailable()) 
			continue;

		char const *key = f.GetNextToken();
		char const *equals = f.GetNextToken();
		char const *val = f.GetNextToken();
		ReleaseAssert(key && equals && val, "Syntax error in prefs file '%s', line %d", filename, f.m_lineNum);
		ReleaseAssert(stricmp(equals, "=") == 0, "Missing equals in prefs file '%s', line %d", filename, f.m_lineNum);

		PrefsItem *item = GetItem(key);
		ReleaseAssert(item, "Unknown item '%s' in prefs file '%s', line %d", key, filename, f.m_lineNum);
		item->m_value = atoi(val);
	}

	return true;
}


bool PrefsManager::Save()
{
	// Now use m_fileText as a template to write most of the items
	FILE *out = fopen(m_filename.m_str, "w");
	if (!out)
		return false;

	for (int i = 0; i < m_items.Size(); ++i)
	{
		PrefsItem *item = m_items[i];
		fprintf(out, "%s = %d\n", item->m_key, item->m_value);
	}

	fclose(out);
    return true;
}


int PrefsManager::GetInt(char const *key)
{
	PrefsItem *item = GetItem(key);
	DebugAssert(item);
	return item->m_value;
}
