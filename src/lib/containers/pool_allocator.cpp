#include "lib/universal_include.h"

#include <stdlib.h>

#include "lib/debug_utils.h"


static const int IS_USED_MARKER = -1;
static const int IS_LAST_MARKER = -2;


template <class T>
PoolAllocator<T>::PoolAllocator() 
{
	m_size = 0;
	m_pool = NULL;
	m_firstFreeIndex = IS_USED_MARKER;
}


template <class T>
PoolAllocator<T>::~PoolAllocator()
{
	m_size = 0;
	delete [] m_pool;
	m_firstFreeIndex = IS_USED_MARKER;
}


template <class T>
void PoolAllocator<T>::Grow()
{
	if (m_size == 0)
	{
		m_size = 8;
		m_pool = new PoolItem<T> [m_size];
		m_firstFreeIndex = 0;
		for (int i = 0; i < m_size; i++)
			m_pool[i].m_nextFreeIndex = i + 1;
		m_pool[m_size - 1].m_nextFreeIndex = IS_LAST_MARKER;
	}
	else
	{
		PoolItem<T> *newPool = new PoolItem<T> [m_size * 2];

		// Copy the old pool contents to the first half of the new pool
		for (int i = 0; i < m_size; i++)
		{
			newPool[i].m_nextFreeIndex = IS_USED_MARKER;
			newPool[i].m_itemData = m_pool[i].m_itemData;
		}

		// Create free list in second half of the new pool
		for (int i = m_size; i < m_size * 2; i++)
			newPool[i].m_nextFreeIndex = i + 1;
		newPool[m_size * 2 - 1].m_nextFreeIndex = IS_LAST_MARKER;

		delete [] m_pool;
		m_pool = newPool;
		m_firstFreeIndex = m_size;
		m_size *= 2;
	}
}


template <class T>
void PoolAllocator<T>::MarkNotUsed(unsigned index)
{
    DebugAssert(index < m_size);
    
    m_pool[index].m_nextFreeIndex = m_firstFreeIndex;
	m_firstFreeIndex = index;
}


template <class T>
T *PoolAllocator<T>::GetFree()
{
	if (m_firstFreeIndex == IS_USED_MARKER)
		Grow();

	// Update the free list
	int freeIndex = m_firstFreeIndex;
	m_firstFreeIndex = m_pool[freeIndex].m_nextFreeIndex;
	
	if (m_firstFreeIndex == IS_LAST_MARKER)
		m_firstFreeIndex = IS_USED_MARKER;

	// Mark the chosen item as used
	m_pool[freeIndex].m_nextFreeIndex = IS_USED_MARKER;

	return &(m_pool[freeIndex].m_itemData);
}


template <class T>
T *PoolAllocator<T>::GetPointer(unsigned index)
{
	DebugAssert(index < m_size);
	DebugAssert(m_pool[index].m_nextFreeIndex == IS_USED_MARKER);

	return &(m_pool[index].m_itemData);
}


template <class T>
bool PoolAllocator<T>::IsIndexUsed(unsigned index) const
{
	if (index >= m_size)
		return false;

	if (m_pool[index].m_nextFreeIndex == IS_USED_MARKER)
		return true;

	return false;
}
