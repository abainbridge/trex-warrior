#ifndef INCLUDED_POOL_ALLOCATOR_H
#define INCLUDED_POOL_ALLOCATOR_H


// PoolAllocator uses an array to provide storage for a number of items of type T.
// * Items can be allocated and released in any order.
// * When an allocation is requested but the array is full, then the array size is doubled.
// * A free list to efficiently find the next free item.
// * A flag is set on used items in the pool so that you can efficiently iterate across all the used items.
// * The pool size never shrinks.


template <class T>
struct PoolItem
{
	T m_itemData;
	int m_nextFreeIndex;	// -1 if this item is used (ie not on the free list). We can use this to iterate across the used items in the pool efficiently.
							// -2 if this item is the last item in the free list.
};


template <class T>
class PoolAllocator
{
protected:
	unsigned m_size;		// Current number of items in pool
	PoolItem<T> *m_pool;
	int m_firstFreeIndex;

	void Grow();

public:
	PoolAllocator();
	~PoolAllocator();

	inline int Size() const { return m_size; }

	inline void MarkNotUsed(unsigned index);
	inline bool IsIndexUsed(unsigned index ) const;	// Returns true if the index contains used data
    
    inline T *GetFree();
    inline T *GetPointer(unsigned index);
};

#include "pool_allocator.cpp"

#endif
