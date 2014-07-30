#include "lib/universal_include.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <crtdbg.h>
#include "debug_utils.h"

#include "app.h"


void DebugOut(char *_fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start (ap, _fmt);
    vsprintf(buf, _fmt, ap);
    OutputDebugString(buf);
}


void ReleaseAssert(bool _condition, char const *_fmt, ...)
{
	if (!_condition)
	{
		char buf[512];
		va_list ap;
		va_start (ap, _fmt);
		vsprintf(buf, _fmt, ap);
		ShowCursor(true);
		MessageBox(NULL, buf, "Fatal Error", MB_OK);
#ifndef _DEBUG
		exit(-1);
#else
		_ASSERT(_condition);
#endif
	}
}


int g_newReportingThreshold = -1; // All memory allocations larger than this will be reported


void SetNewReportingThreshold(int _size)
{
	g_newReportingThreshold = _size;
}


#ifdef OVERLOADED_NEW
#undef new
void *operator new (unsigned _size, char const *_filename, int _line)
{
	void *p = malloc(_size);

	if ((signed)_size > g_newReportingThreshold)
	{
		DebugOut("%s line %d: %d bytes at 0x%0x\n", _filename, _line, _size, p);
	}
	DebugAssert(p);

	return p;
}


void *operator new[] (unsigned _size, char const *_filename, int _line)
{
	void *p = malloc(_size);

	if ((signed)_size > g_newReportingThreshold)
	{
		DebugOut("%s line %d: %d bytes at 0x%0x\n", _filename, _line, _size, p);
	}
	DebugAssert(p);

	return p;
}


void operator delete (void *p, char const *_filename, int _line)
{
	free(p);
}


void operator delete[] (void *p, char const *_filename, int _line)
{
	free(p);
}


#endif // #ifdef OVERLOADED_NEW


void PrintMemoryLeaks ()
{
#ifdef TRACK_MEMORY_LEAKS
    char filename[] = "memory.log";

    OFSTRUCT ofstruct;
    HFILE file = OpenFile ( filename,
                            &ofstruct,
                            OF_CREATE );
                     
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, (void *) file);
  
    _CrtDumpMemoryLeaks ();

    _lclose ( file );
#endif
}


unsigned *getRetAddress(unsigned *mBP)
{
#ifdef WIN32
	unsigned *retAddr;

	__asm {
		mov eax, [mBP]
		mov eax, ss:[eax+4];
		mov [retAddr], eax
	}

	return retAddr;
#else
	unsigned **p = (unsigned **) mBP;
	return p[1];
#endif
}
