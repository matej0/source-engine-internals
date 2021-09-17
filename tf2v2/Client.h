#pragma once
#include "SDK.h"
#include "Util.h"
#include <deque>

struct IBoneSetup;

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
void __fastcall Hooked_FrameStageNotify(PVOID ClientMode, int edx, ClientFrameStage_t Frame);
void __stdcall Hooked_OverrideView(CViewSetup* pSetup);
void __stdcall Hooked_LevelInitPreEntity(const char* map);
bool __fastcall Hooked_IsPlayingBack(PVOID ecx, int edx);
bool __fastcall Hooked_ShouldDrawFog(PVOID ecx, PVOID edx);
void __fastcall Hooked_GetOuterAbsVelocity(PVOID ecx, PVOID edx, Vector &vel);
void __fastcall Hooked_UpdatePlayerList(PVOID ecx, PVOID edx);
void __fastcall Hooked_MaintainSequenceTransitions(PVOID ecx, PVOID edx, IBoneSetup &boneSetup, float flCycle, Vector pos[], Quaternion q[]);
void __fastcall Hooked_EstimateAbsVelocity(CBaseEntity* thisptr, PVOID edx, Vector &vel);
bool __fastcall Hooked_Teleported(PVOID ecx, PVOID edx);
void __fastcall Hooked_AddFlag(PVOID ecx, PVOID edx, int flag);
void __fastcall Hooked_UpdateClientSideAnimations(CBaseEntity* ecx, PVOID edx);
void __fastcall Hooked_RunCommand(CPrediction *pPrediction, PVOID edx, CBaseEntity *player, CUserCmd *ucmd, IMoveHelper *moveHelper);
void __fastcall Hooked_ItemPostFrame(CBaseEntity* ecx, PVOID edx);

/*
int GetTarget(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	int iBestTarget = -1;
	float flDistToBest = 99999.f;
	Vector vLocal = pLocal->GetEyePosition();

	/*float Time = (gInts.Globals->curtime - flLastAimTime);

	//if its been 200ms since the last time we aimed at a target, just drop it and recalculate.
	if (Time <= 0.2f)
	{
		CBaseEntity* pLastEntity = GetBaseEntity(iLastTarget);

		if (pLastEntity && pLastEntity->IsAlive() && pLastEntity->GetTeam() != pLocal->GetTeam() && !pLastEntity->IsDormant())
		{
			float TempFOV = gMath.GetFOV(pCommand->viewangles, vLocal, pLastEntity->GetEyePosition());

			if (TempFOV < flDistToBest && TempFOV < gPlayerVars.iAimbotFov)
			{
				if (bIsVisible(pLastEntity, pLocal, vLocal, GetHitbox(pLocal, pLastEntity, pCommand)))
				{
					flDistToBest = TempFOV;
					flLastAimTime = gInts.Globals->curtime;
					return iLastTarget;
				}
			}
		}
	}

	//restoring these.
	//iLastTarget = -1;
	//flDistToBest = 99999.f;

for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
{
	if (i == me)
		continue;

	CBaseEntity* pEntity = GetBaseEntity(i);

	if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->GetTeam() == pLocal->GetTeam())
		continue;

	Vector iBestHitbox;// = GetHitbox(pLocal, pEntity, pCommand);

	if (iBestHitbox == Vector(0, 0, 0))
		continue;

	float flFOV = gMath.GetFOV(pCommand->viewangles, vLocal, pEntity->GetEyePosition());

	if (flFOV < flDistToBest && flFOV < gPlayerVars.iAimbotFov)
	{
		flDistToBest = flFOV;
		iBestTarget = i;
		//flLastAimTime = gInts.Globals->curtime;
	}
}

//iLastTarget = iBestTarget;


return iBestTarget;
}

bool CanStab(CBaseEntity* pLocal, CGameTrace* pTrace)
{
	static Vector vecSwingMins(-18, -18, -18);
	static Vector vecSwingMaxs(18, 18, 18);
	float flKnifeRange = 48.f;

	Vector vLocalViewAngles;
	gInts.Engine->GetViewAngles(vLocalViewAngles);

	Vector vForward;
	AngleVectors(vLocalViewAngles, &vForward);

	Vector vStart = pLocal->GetEyePosition();
	Vector vEnd = vStart + (vForward * flKnifeRange);

	CTraceFilter filter;
	filter.pSkip = pLocal;

	UTIL_TraceLine(vStart, vEnd, MASK_SOLID, &filter, pTrace);

	if (pTrace->fraction >= 1.0f)
	{
		//tf2 does a hull trace if the trace ray doesnt hit anything

		UTIL_TraceHull(vStart, vEnd, vecSwingMins, vecSwingMaxs, MASK_SOLID, &filter, pTrace);

		//tf2 calls "FindHullIntersection" which ive rebuilt but i realized this isnt necessary, they use it to see if you hit nonplayer entities or something.
		return pTrace->fraction < 1.0f;
	}

	return pTrace->fraction <= 1.0f;
}

void Knife(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	if (!pLocal || !pLocal->IsAlive())
		return;

	if (pLocal->GetClassNum() != TF2_Spy)
		return;

	if (pLocal->GetActiveWeapon()->GetSlot() != TF_WEAPONSLOT_MELEE)
		return;

	CGameTrace trace;
	if (CanStab(pLocal, &trace))
	{
		if (trace.m_pEnt && trace.m_pEnt->IsPlayer())
		{
			if (trace.m_pEnt->GetTeam() != pLocal->GetTeam())
			{
				if (pLocal->IsBehindTarget(trace.m_pEnt))
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
		}
	}
}

void Aimbot(CUserCmd* pCommand, CBaseEntity* pLocal)
{
	if (!gPlayerVars.bAimbot)
		return;

	if (!GetAsyncKeyState(VK_XBUTTON2))
		return;

	if (!pLocal->GetActiveWeapon() || pLocal->HoldingProjectileWeapon() && !pLocal->CanFireWithCurrentWeapon())
		return;

	CBaseEntity* pEntity = GetBaseEntity(GetTarget(pLocal, pCommand));

	if (!pEntity)
		return;

	Vector iBestHitbox;//GetHitbox(pLocal, pEntity, pCommand);

	if (iBestHitbox == Vector(0, 0, 0))
		return;

	Vector vOldView = pCommand->viewangles;
	float flOldSidemove = pCommand->sidemove;
	float flOldForwardmove = pCommand->forwardmove;

	Vector vecStart = pLocal->GetEyePosition();

	Vector vecTarget = iBestHitbox;

	Vector vecAngle;

	VectorAngles((vecTarget - vecStart), vecAngle);

	ClampAngle(vecAngle);

	bool bHuntsman = (pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_Huntsman ||
		pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_CompoundBow ||
		pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_FestiveHuntsman
		&& pLocal->GetActiveWeapon()->GetChargeBeginTime() != 0);

	//set aimbot angle.
	if (gPlayerVars.bSilent)
		pCommand->viewangles = vecAngle;
	else
		gInts.Engine->SetViewAngles(vecAngle);

	if (gPlayerVars.bAutoshoot)
	{
		if (bHuntsman && pCommand->buttons & IN_ATTACK)
			pCommand->buttons &= ~IN_ATTACK;
		else
			pCommand->buttons |= IN_ATTACK;
	}

	if (gPlayerVars.bSilent && !bHuntsman)
	{
		if (pCommand->buttons & IN_ATTACK && pLocal->CanFireWithCurrentWeapon())
		{
			*gPlayerVars.bSendPacket = false;
		}
		else
		{
			*gPlayerVars.bSendPacket = true;
			pCommand->viewangles = vOldView;
			pCommand->sidemove = flOldSidemove;
			pCommand->forwardmove = flOldForwardmove;
		}
	}

	pCommand->tick_count = TIME_TO_TICKS(pEntity->GetSimulationTime() + GetLerpTime());
}*/

/*Vector GetHitbox(CBaseEntity* pLocal, CBaseEntity* pEntity, CUserCmd* pCommand)
{
	Vector vecHitbox = Vector();

	if (pLocal->HoldingProjectileWeapon())
	{
		Vector vecVelocity;
		pEntity->EstimateAbsVelocity(vecVelocity);

		if (pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_Huntsman || pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_CompoundBow || pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_FestiveHuntsman)
		{
			vecHitbox = pEntity->GetHitboxPositionCustom(0, pMatrix[pEntity->GetIndex()]);
		}
		else
		{
			if (pLocal->GetClassNum() == TF2_Soldier || !(pEntity->GetFlags() & FL_ONGROUND))
				vecHitbox = pEntity->GetOrigin();
			else
				vecHitbox = pEntity->GetHitboxPositionCustom(3, pMatrix[pEntity->GetIndex()]); //aim at the center otherwise
		}

		if (pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_Huntsman || pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_FestiveHuntsman || pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_CompoundBow)
		{
			float _time = (pLocal->GetEyePosition().DistTo(vecHitbox) / pLocal->GetActiveWeapon()->GetArrowSpeed());
			float comp = (0.5f  * 800 * pLocal->GetActiveWeapon()->GetArrowGravity()) * std::pow(_time, 2);

			vecHitbox.x += (vecVelocity.x * _time);
			vecHitbox.y += (vecVelocity.y * _time);
			vecHitbox.z += (vecVelocity.z * _time) + comp;


		}
		else
		{
			float flTime = (pLocal->GetEyePosition().DistTo(vecHitbox) / pLocal->GetActiveWeapon()->GetProjectileSpeed());

			vecHitbox.x += (vecVelocity.x * flTime);
			vecHitbox.y += (vecVelocity.y * flTime);

			//only handle gravity if in air.
			if (!(pEntity->GetFlags() & FL_ONGROUND))
			{
				vecHitbox.z += ((0.5 * 800) * powf(flTime, 2)) + (vecVelocity.z * flTime);
			}
		}
	}
	else
	{
		if (gPlayerVars.bHitscan)
		{

		}
		else
		{
			if (pLocal->GetCond() & TFCond_Zoomed && pLocal->GetClassNum() == TF2_Sniper && pLocal->GetActiveWeapon()->GetSlot() == TF_WEAPONSLOT_PRIMARY ||
				pLocal->GetClassNum() == TF2_Spy && pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_Ambassador)
			{
				vecHitbox = pEntity->GetHitboxPositionCustom(0, pMatrix[pEntity->GetIndex()]);
			}
			else
			{
				vecHitbox = pEntity->GetHitboxPositionCustom(3, pMatrix[pEntity->GetIndex()]);
			}
		}
	}

	if (vecHitbox.IsZero() || !UTIL_IsEntityVisible(pEntity, pLocal, pLocal->GetEyePosition(), vecHitbox))
		return Vector(0, 0, 0);

	return vecHitbox;
}*/

