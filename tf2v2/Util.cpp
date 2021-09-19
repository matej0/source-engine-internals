#include "Util.h"

//"rebuilt"
void UTIL_TraceLine(Vector vecAbsStart, Vector vecAbsEnd, unsigned int mask, CTraceFilter* pFilter, CGameTrace *tr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);

	gInts.EngineTrace->TraceRay(ray, mask, pFilter, tr);
}

void UTIL_TraceHull(Vector& vecAbsStart, Vector& vecAbsEnd, Vector &mins, Vector &maxs, unsigned int mask, CTraceFilter* pFilter, CGameTrace *tr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd, mins, maxs);

	gInts.EngineTrace->TraceRay(ray, mask, pFilter, tr);
}

void UTIL_FindHullIntersection(Vector &vecSrc, CGameTrace &tr, Vector &mins, Vector &maxs, CBaseEntity *pIgnoreEntity)
{
	int	i, j, k;
	CGameTrace tmpTrace;
	Vector vecEnd;
	float distance = 1e6f;
	Vector minmaxs[2] = { mins, maxs };
	Vector vecHullEnd = tr.endpos;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc) * 2);

	CTraceFilter filter;
	filter.pSkip = pIgnoreEntity;

	Ray_t ray;
	ray.Init(vecSrc, vecHullEnd);

	gInts.EngineTrace->TraceRay(ray, MASK_SOLID, &filter, &tmpTrace);

	if (tmpTrace.fraction < 1.0)
	{
		tr = tmpTrace;
		return;
	}

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			for (k = 0; k < 2; k++)
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				Ray_t ray2;
				ray2.Init(vecSrc, vecHullEnd);

				gInts.EngineTrace->TraceRay(ray2, MASK_SOLID, &filter, &tmpTrace);

				if (tmpTrace.fraction < 1.0)
				{
					float thisDistance = (tmpTrace.endpos - vecSrc).Length();
					if (thisDistance < distance)
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

bool UTIL_IsEntityVisible(CBaseEntity *pLocal, CBaseEntity *pEntity, Vector src, Vector dst)
{
	CGameTrace trace;
	CTraceFilter filter;
	filter.pSkip = pLocal;

	UTIL_TraceLine(src, dst, (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);

	return (trace.m_pEnt == pEntity || trace.fraction == 1.f);
}


float UTIL_GetLerpTime()
{
	//https://github.com/lua9520/source-engine-2018-hl2_src/blob/3bf9df6b2785fa6d951086978a3e66f49427166a/game/server/gameinterface.cpp#L2838
	/*int m_nUpdateRate = gConvars.cl_updaterate->GetInt();

	static const ConVar *pMinUpdateRate = gInts.Cvar->FindVar("sv_minupdaterate");
	static const ConVar *pMaxUpdateRate = gInts.Cvar->FindVar("sv_maxupdaterate");

	if (pMinUpdateRate && pMaxUpdateRate)
		m_nUpdateRate = clamp(m_nUpdateRate, (int)pMinUpdateRate->GetFloat(), (int)pMaxUpdateRate->GetFloat());

	float flLerpRatio = gConvars.cl_interp_ratio->GetFloat();

	if (flLerpRatio == 0)
		flLerpRatio = 1.0f;

	float flLerpAmount = gConvars.cl_interp->GetFloat();

	static const ConVar *pMin = gInts.Cvar->FindVar("sv_client_min_interp_ratio");
	static const ConVar *pMax = gInts.Cvar->FindVar("sv_client_max_interp_ratio");

	if (pMin && pMax && pMin->GetFloat() != -1)
	{
		flLerpRatio = clamp(flLerpRatio, pMin->GetFloat(), pMax->GetFloat());
	}
	else
	{
		if (flLerpRatio == 0)
		flLerpRatio = 1.0f;
	}

	return fmax(flLerpAmount, flLerpRatio / m_nUpdateRate);*/

	if (!gConvars.cl_updaterate)
	{
		gConvars.cl_interp = gInts.Cvar->FindVar("cl_interp");
		gConvars.cl_updaterate = gInts.Cvar->FindVar("cl_updaterate");
		gConvars.cl_interp_ratio = gInts.Cvar->FindVar("cl_interp_ratio");
		gConvars.sv_minupdaterate = gInts.Cvar->FindVar("sv_minupdaterate");
		gConvars.sv_maxupdaterate = gInts.Cvar->FindVar("sv_maxupdaterate");
	}

	float update_rate = clamp(gConvars.cl_updaterate->GetFloat(), gConvars.sv_minupdaterate->GetFloat(), gConvars.sv_maxupdaterate->GetFloat());
	float lerp_ratio = gConvars.cl_interp_ratio->GetFloat();

	if (lerp_ratio == 0.0f)
		lerp_ratio = 1.0f;

	float lerp_amount = gConvars.cl_interp->GetFloat();

	ConVar* pMin = gInts.Cvar->FindVar("sv_client_min_interp_ratio");
	ConVar* pMax = gInts.Cvar->FindVar("sv_client_max_interp_ratio");

	if (pMin && pMax && pMin->GetFloat() != -1.0f)
	{
		lerp_ratio = clamp(lerp_ratio, pMin->GetFloat(), pMax->GetFloat());
	}
	else
	{
		if (lerp_ratio == 0.0f)
			lerp_ratio = 1.0f;
	}

	float ret = fmax(lerp_amount, lerp_ratio / update_rate);

	return ret;
}



UINT UTIL_GetVirtual(PVOID pClass, UINT nIndex)
{
	return (unsigned int)(*(int**)pClass)[nIndex];
}