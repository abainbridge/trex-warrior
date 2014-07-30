#ifndef INCLUDED_PREFERENCES_H
#define INCLUDED_PREFERENCES_H


#include "lib/containers/list.h"
#include "lib/lite_string.h"


class PrefsItem
{
public:
	char const *m_key;
	int m_value;

	PrefsItem(char const *key, int val)
	{
		m_key = key;
		m_value = val;
	}
};


class PrefsManager
{
private:
    LiteString m_filename;
	List <PrefsItem *> m_items;	

	PrefsItem *GetItem(char const *key);
//	void SaveItem(FILE *out, PrefsItem *item);

public:
	void RegisterInt(char const *key, int val);

	bool Load(char const *filename);

	bool Save();

	int  GetInt(char const *key);
	void SetInt(char const *key, int val);   
};


extern PrefsManager g_prefsManager;


#endif
