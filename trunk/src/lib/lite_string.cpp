#include "lib/universal_include.h"
#include "lite_string.h"

#include <stdlib.h>
#include <string.h>


LiteString::LiteString(char const *other)
{
	m_str = strdup(other);
}


LiteString::LiteString(LiteString const &other) 
{ 
	m_str = strdup(other.m_str); 
}


LiteString::~LiteString()
{
	free(m_str);
}


void LiteString::operator = (char const *other)
{
	free(m_str);
	m_str = strdup(other);
}


void LiteString::operator += (char const *other)
{
	char *newStr = (char*)malloc(strlen(m_str) + strlen(other) + 1);
	strcpy(newStr, m_str);
	strcat(newStr, other);
	free(m_str);
	m_str = newStr;
}


void LiteString::ToUpper()
{
	if (!m_str)
		return;

	char *c = m_str;
	while (*c != '\0')
	{
		*c = toupper(*c);
		c++;
	}
}


void LiteString::ToLower()
{
	if (!m_str)
		return;

	char *c = m_str;
	while (*c != '\0')
	{
		*c = tolower(*c);
		c++;
	}
}
