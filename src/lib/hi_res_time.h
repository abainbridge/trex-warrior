#ifndef INCLUDED_HI_RES_TIME_H
#define INCLUDED_HI_RES_TIME_H


extern double g_gameTime;					  // Updated from GetHighResTime every frame
extern float g_advanceTime;
#define g_predictionTime (0.0f)
#define MAX_ADVANCE_TIME (1.0/50.0)


void HiresTimeInit();
double HiresTimeGet();        // Return value in seconds
void HiresTimeUpdate();			// Call this at the start of each frame's advance code

#endif
