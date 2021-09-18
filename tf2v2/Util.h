#pragma once
#include "SDK.h"
#include "Math.h"


//valve style util functions
void UTIL_TraceLine(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, CTraceFilter* pFilter, CGameTrace *tr);
void UTIL_TraceHull(Vector& vecAbsStart, Vector& vecAbsEnd, Vector &mins, Vector &maxs, unsigned int mask, CTraceFilter* pFilter, CGameTrace *tr);
void UTIL_FindHullIntersection(Vector &vecSrc, CGameTrace &tr, Vector &mins, const Vector &maxs, CBaseEntity *pIgnoreEntity);
bool UTIL_IsEntityVisible(CBaseEntity *pLocal, CBaseEntity *pEntity, Vector &src, Vector &dst);
float UTIL_GetLerpTime();
UINT UTIL_GetVirtual(PVOID pClass, UINT nIndex);

