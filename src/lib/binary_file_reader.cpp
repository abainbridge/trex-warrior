#include "lib/universal_include.h"
#include "lib/binary_file_reader.h"

#ifdef WIN32
#include <io.h>
#endif

#include <stdio.h>
#include <string.h>

#include "lib/debug_utils.h"



BinaryFileReader::BinaryFileReader()
{
	m_eof = false;
	m_file = NULL;
}


BinaryFileReader::~BinaryFileReader()
{
	if (m_file)
		fclose(m_file);
}


bool BinaryFileReader::Open(char const *filename)
{
	m_file = fopen(filename, "rb");
	if (m_file) 
		return true;
	return false;
}


signed char BinaryFileReader::ReadS8()
{
	int c = fgetc(m_file);
	if (c == EOF)
		m_eof = true;
	return c;
}


unsigned char BinaryFileReader::ReadU8()
{
	int c = fgetc(m_file);
	if (c == EOF)
		m_eof = true;
	return c;
}


short BinaryFileReader::ReadS16()
{
	int b1 = fgetc(m_file);
	int b2 = fgetc(m_file);
	
	if (b1 == EOF || b2 == EOF)
		m_eof = true;

	return ((b2 << 8) | b1);
}


int BinaryFileReader::ReadS32()
{
	int b1 = fgetc(m_file);
	int b2 = fgetc(m_file);
	int b3 = fgetc(m_file);
	int b4 = fgetc(m_file);

	if (b1 == EOF || b2 == EOF || b3 == EOF || b4 == EOF)
		m_eof = true;

	return ((b4 << 24) | (b3 << 16) | (b2 << 8) | b1);
}


unsigned int BinaryFileReader::ReadBytes(unsigned int _count, unsigned char *_buffer)
{
	int bytesRead = fread(_buffer, 1, _count, m_file);
	if (bytesRead < _count)
		m_eof = true;

	return bytesRead;
}
