#ifndef INCLUDED_HASH_TABLE_H
#define INCLUDED_HASH_TABLE_H


// Implements a simple hash table with null terminated char arrays for keys.
// - You can initialize it to any size greater than 1. 
// - PushBack() checks if the table is more than half full. If it is the table 
//	 size is doubled and the data is re-indexed. 
// - The table never shrinks. 
// - The hash collision rule is just to increment through the table until
//   a free slot is found
// - Looking up a key is O(1) even when key does not exist

// *** Note on memory usage ***
// This class is optimized for speed not memory usage. The RAM used by m_data
// will always be between (2 x num slots used x sizeof(T)) and 
// (4 x num slots used x sizeof(T))


template <class T>
class HashTable
{
protected:
	char					**m_keys;
	T						*m_data;
	unsigned int			m_slotsFree;
	unsigned int			m_size;
	unsigned int			m_mask;

	unsigned int	HashFunc	(char const *key) const;
	void			Grow		();
	unsigned int	GetInsertPos(char const *key) const;			// Returns the index of the position where key should be placed

public:
	HashTable		();
	~HashTable		();

	int				GetIndex	(char const *key) const;			// Returns -1 if key isn't present

	int				PushBack	(char const *key, T const &data); // Returns slot used
	T				GetData		(char const *key, T const &_default = NULL) const;
	T *				GetPointer	(char const *key) const;
	void			RemoveData	(char const *key);					// Client still MUST delete if data is a pointer
	void			RemoveData	(unsigned int index);				// Client still MUST delete if data is a pointer

	void			Empty		();

	bool			ValidIndex	(unsigned int index) const;
	unsigned int	Size		() const;							// Returns total table size, NOT number of slots used

	T				operator [] (unsigned int index) const;
};


#include "hash_table.cpp"


#endif
