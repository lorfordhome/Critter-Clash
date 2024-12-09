#include "D3DUtil.h"

static float gTime = 0;

float GetClock()
{
	return gTime;
}

void AddSecToClock(float sec)
{
	gTime += sec;
}





