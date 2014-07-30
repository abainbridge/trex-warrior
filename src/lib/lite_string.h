#ifndef INCLUDED_LITE_STRING_H
#define INCLUDED_LITE_STRING_H


#include <stdlib.h> // For NULL


class LiteString
{
public:
	char *m_str;

	LiteString() { m_str = NULL; }
	
	LiteString(char const *other);
	LiteString(LiteString const &other);

	~LiteString();

	void operator = (char const *other);
	void operator = (LiteString &other) { *this = other.m_str; }

	void operator += (char const *other);
	void operator += (LiteString &other) { *this += other.m_str; }

	void ToUpper();
	void ToLower();
};


#endif
