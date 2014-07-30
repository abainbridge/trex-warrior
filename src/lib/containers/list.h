#ifndef INCLUDED_LIST_H
#define INCLUDED_LIST_H


// This is a sort-of a hybrid or a vector and a list.
// List like features:
//  * You can add and remove data to/from anywhere in O(1) time
//
// Vector like features:
//  * You can access items by index, eg foo[12]. This is fast if the last item
//    accessed was near 12.


template <class T>
struct ListItem
{
    T		  m_data;
    ListItem *m_next;
    ListItem *m_prev;
};


template <class T>
class List
{
protected:
    ListItem<T> *m_first;
	ListItem<T> *m_last;

	mutable ListItem<T> *m_cached;      // \ Used to get quick access to items near the
	mutable int m_cachedIndex;          // / last item that was accessed (common)

	int         m_numItems;

protected:
	inline ListItem<T> *GetItem(int index) const;

public:
    List();
	~List();

	inline void PushBack	    (const T &data);        // O(1)
	void		PushFront	    (const T &data);	    // O(1)
	void		PushAtIndex	    (const T &data, int index);

    T   		RemoveData		(int index);			// O(n) unless nearly sequential
	inline T	RemoveDataAtEnd	();
    void		Clear			();						// Removes all items
    void		ClearAndDelete	();						// Destructs and then removes all items

    inline T   *GetPointer		(int index) const;	    // \ O(n) unless nearly sequential
    inline T const operator []  (int index) const;      // /
    int			FindData		(const T &data);		// -1 means 'not found'
    inline bool ValidIndex		(int index) const;
 
    inline int	Size			() const;
};


#include "list.cpp"


#endif
