#include "lib/universal_include.h"
#include "lib/hi_res_time.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


static double g_tickInterval = 1.0;
double g_gameTime = 0.0;
float g_advanceTime = 0.0;


void HiresTimeInit()
{
	// Start be getting the frequency the Performance Counter uses.
	// We need to use the Performance Counter to calibrate the Pentium
	// Counter, if we are going to use it.
    LARGE_INTEGER count;
    QueryPerformanceFrequency(&count);
    g_tickInterval = 1.0 / (double)count.QuadPart;
}


double HiresTimeGet()
{
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return (double)count.QuadPart * g_tickInterval;
}


void HiresTimeUpdate()
{
	double realTime = HiresTimeGet();
	g_advanceTime = float(realTime - g_gameTime);
	if (g_advanceTime > 0.1f)
		g_advanceTime = 0.1f;
	g_gameTime = realTime;
}
