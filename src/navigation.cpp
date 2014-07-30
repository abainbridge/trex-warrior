#include "lib/universal_include.h"
#include "navigation.h"

#include "lib/hi_res_time.h"
#include "lib/text_file_reader.h"
#include "lib/vector2.h"


NavRoute::NavRoute(char const *name)
{
	m_name = name;
	Load();
}


NavRoute::~NavRoute()
{
	m_wayPoints.ClearAndDelete();
}


static LiteString GetFullFilename(LiteString name)
{
	LiteString full = "data/nav/";
	full += name;
	full += ".nav";
	return full;
}


bool NavRoute::Load()
{
	LiteString path = GetFullFilename(m_name);
	TextFileReader i(path.m_str);
	if (!i.IsOpen())
		return false;

	// File format is one line per point
	// Line contains <float> <float>

	while (i.ReadLine())
	{
		char const *xStr = i.GetNextToken();
		if (!xStr)
			break;
		char const *yStr = i.GetNextToken();
		if (!yStr)
			return false;

		Vector2 *p = new Vector2(atof(xStr), atof(yStr));
		m_wayPoints.PushBack(p); 
	}

	return true;
}


bool NavRoute::Save()
{
	LiteString path = GetFullFilename(m_name);
	FILE *o = fopen(path.m_str, "w");
	if (!o)
		return false;

	for (int i = 0; i < m_wayPoints.Size(); i++)
	{
		Vector2 *p = m_wayPoints[i];
		fprintf(o, "%.3f %.3f\n", p->x, p->y);
	}

	fclose(o);
	return true;
}


void NavRoute::NextPos(NavRouteFollower *nrf)
{
	Vector2 *nextWayPoint = m_wayPoints[nrf->targetWayPointIdx];
	nrf->front = *nextWayPoint - nrf->pos;
	if (nrf->front.Len() < nrf->maxSpeed * MAX_ADVANCE_TIME)
	{
		nrf->targetWayPointIdx++;
		nrf->targetWayPointIdx %= m_wayPoints.Size();
		nextWayPoint = m_wayPoints[nrf->targetWayPointIdx];
		nrf->front = *nextWayPoint - nrf->pos;
	}
	
	nrf->front.Normalize();
}
