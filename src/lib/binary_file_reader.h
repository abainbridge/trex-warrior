#ifndef INCLUDED_BINARY_FILE_READER_H
#define INCLUDED_BINARY_FILE_READER_H

#include <stdio.h>


class BinaryFileReader
{
protected:
	FILE			*m_file;
	bool			m_eof;

public:
	BinaryFileReader			();
	~BinaryFileReader			();

    bool			Open		(char const *filename);
	bool			IsEof		() { return m_eof; }

	signed char		ReadS8		();
	short			ReadS16		();
	int				ReadS32		();
	unsigned char	ReadU8		();
    unsigned short	ReadU16		() { return (unsigned)ReadS16(); }
    unsigned int	ReadU32		() { return (unsigned)ReadS32(); }

	unsigned int	ReadBytes	(unsigned int count, unsigned char *buffer);
};


#endif
