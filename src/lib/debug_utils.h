#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <crtdbg.h>
#include <stdio.h>

void DebugOut(char *fmt, ...);

void ReleaseAssert(bool condition, char const *_fmt, ...);

#ifdef _DEBUG
  #ifdef _MSC_VER
	#define DebugAssert(x) \
		if(!(x)) { \
			ShowCursor(true); \
			::ClipCursor(NULL); \
			_CrtDbgBreak(); \
		}
  #else
    #define DebugAssert(x) { assert(x); }
  #endif
#else
	#define DebugAssert(x)
#endif


void WriteSystemReport(FILE *_out);


#endif

