#include <string.h>
#include <stdlib.h> // for free()

#include "debug_utils.h"
#include "hash_table.h"


//*****************************************************************************
// Protected Functions
//*****************************************************************************

template <class T>
unsigned int HashTable<T>::HashFunc(char const *key) const
{
	unsigned short rv = 0;

	while (key[0] && key[1])
	{
		rv += key[0] & 0xDF;
		rv += ~(key[1] & 0xDF);	// 0xDF removes the case bit
		key += 2;
	}

	if (key[0])
		rv += *key & 0xDF;

	return rv & m_mask;
}


template <class T>
void HashTable<T>::Grow()
{
	ReleaseAssert(m_size < 65536, "Hashtable grew too large");

	unsigned int oldSize = m_size;
	m_size *= 2;
	m_mask = m_size - 1;
	char **oldKeys = m_keys;
	m_keys = new char *[m_size];
	T *oldData = m_data;
	m_data = new T [m_size];

	// Set all m_keys' pointers to NULL
	memset(m_keys, 0, sizeof(char *) * m_size);
	memset(m_data, 0, sizeof(T) * m_size);

	for (unsigned int i = 0; i < oldSize; ++i)
	{
		if (oldKeys[i])
		{
			unsigned int newIndex = GetInsertPos(oldKeys[i]);
			m_keys[newIndex] = oldKeys[i];
			m_data[newIndex] = oldData[i];
		}
	}

	m_slotsFree += m_size - oldSize;

	delete [] oldKeys;
	delete [] oldData;
}


template <class T>
unsigned int HashTable<T>::GetInsertPos(char const *_key) const
{
	unsigned int index = HashFunc(_key);

	// Test if the target slot is empty, if not increment until we
	// find an empty one
	while (m_keys[index] != NULL)
	{
		DebugAssert(stricmp(m_keys[index], _key) != 0);
		index++;
		index &= m_mask;
	}

	return index;
}



//*****************************************************************************
// Public Functions
//*****************************************************************************

template <class T>
HashTable<T>::HashTable()
:	m_keys(NULL),
	m_size(4)
{
	m_mask = m_size - 1;
	m_slotsFree = m_size;
	m_keys = new char *[m_size];
	m_data = new T [m_size];

	// Set all m_keys' pointers to NULL
	memset(m_keys, 0, sizeof(char *) * m_size);
	memset(m_data, 0, sizeof(T) * m_size);
}


template <class T>
HashTable<T>::~HashTable()
{
	Empty();

	delete [] m_keys;
	delete [] m_data;
}


template <class T>
void HashTable<T>::Empty()
{
	for (unsigned int i = 0; i < m_size; ++i)
		free(m_keys[i]);
	memset(m_keys, 0, sizeof(char *) * m_size);
	memset(m_data, 0, sizeof(T) * m_size);
}


template <class T>
int HashTable<T>::GetIndex(char const *key) const
{
	unsigned int index = HashFunc(key);

	if (m_keys[index] == NULL)
		return -1;

	while (stricmp(m_keys[index], key) != 0)
	{
		index++;
		index &= m_mask;

		if (m_keys[index] == NULL)
			return -1;
	}

	return index;
}


template <class T>
int HashTable<T>::PushBack(char const *key, T const &data)
{
	// Is it time to grow the table?
	if (m_slotsFree * 2 <= m_size)
		Grow();


	// Do the main insert
	unsigned int index = GetInsertPos(key);
	DebugAssert(m_keys[index] == NULL);
	m_keys[index] = strdup(key);
	m_data[index] = data;
	m_slotsFree--;

	return index;
}


template <class T>
T HashTable<T>::GetData(char const *_key, T const &_default) const
{
	int index = GetIndex(_key);
	if (index >= 0)
		return m_data[index];

	return _default;
}


template <class T>
T *HashTable<T>::GetPointer(char const *_key) const
{
	int index = GetIndex(_key);
	if (index >= 0)
		return &m_data[index];

	return NULL;
}


template <class T>
void HashTable<T>::RemoveData(char const *key)
{
	int index = GetIndex(key);
	if (index >= 0)
		RemoveData(index);
}


template <class T>
void HashTable<T>::RemoveData(unsigned int index)
{
	delete [] m_keys[index];
	m_keys[index] = NULL;
	m_slotsFree++;
}


template <class T>
bool HashTable<T>::ValidIndex(unsigned int index) const
{
	return m_keys[index] != NULL;
}


template <class T>
unsigned int HashTable<T>::Size() const
{
	return m_size;
}


template <class T>
T HashTable<T>::operator [] (unsigned int index) const
{
	return m_data[index];
}
