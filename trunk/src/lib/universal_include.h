#ifndef INCLUDED_UNIVERSAL_INCLUDE_H
#define INCLUDED_UNIVERSAL_INCLUDE_H

//#define PROFILER_ENABLED 1
//#define EDITOR_ENABLED 1

#ifdef _MSC_VER
	#pragma warning(disable: 4018 4244 4305 4800 4996)
	
	#define WIN32_LEAN_AND_MEAN
	#include "windows.h"
#endif

#include <crtdbg.h>
#include <math.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "lib/debug_utils.h"

#endif
