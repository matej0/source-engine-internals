#include "SDK.h"
#include "Client.h"
#include "CDrawManager.h"
#include <intrin.h>
#include "Math.h"
#include "Aimbot.h"

RequiredMath gMath;
BacktrackingData Data[64][12];

bool __fastcall Hooked_ShouldDrawFog(PVOID ecx, PVOID edx)
{
	if ((DWORD)_ReturnAddress() != gVars.dwFogReturnAddress)
	{
		return oFog(ecx);
	}

	return false;
}

void BacktrackPlayer(CUserCmd* cmd, CBaseEntity* pLocal)
{
	if (gVars.bBacktracking)
	{
		int nBestTargetIndex = -1;

		float flBestFov = FLT_MAX;

		if (!pLocal->IsAlive())
			return;

		for (int i = 1; i <= gInts.Globals->maxClients; i++)
		{
			auto pEntity = GetBaseEntity(i);

			if (!pEntity || !pLocal)
				continue;

			if (pEntity == pLocal)
				continue;

			if (pEntity->IsDormant())
				continue;

			if (pEntity->GetTeam() == pLocal->GetTeam())
				continue;

			if (pEntity->IsAlive())
			{
				float flSimulationTime = pEntity->GetSimulationTime();

				Vector vecHeadPosition = pEntity->GetBonePosition(8);

				Data[i][cmd->command_number % int(gVars.nBacktrackTicks)] = BacktrackingData{ flSimulationTime, vecHeadPosition };
				//pEntity->SetupBones(Data[i][cmd->command_number % int(gVars.nBacktrackTicks)].m_Matrix, 128, BONE_USED_BY_ANYTHING, gInts.Globals->curtime);

				Vector ViewDir = gMath.AngleVector(cmd->viewangles + (pLocal->GetAimPunchAngle() * 2.f));

				float FOVDistance = gMath.DistancePointToLine(vecHeadPosition, pLocal->GetEyePosition(), ViewDir);

				if (flBestFov > FOVDistance)
				{
					flBestFov = FOVDistance;
					nBestTargetIndex = i;
				}
			}
		}


		float flBestTargetSimTime;
		if (nBestTargetIndex != -1)
		{
			float flTempFloat = FLT_MAX;
			Vector ViewDir = gMath.AngleVector(cmd->viewangles + (pLocal->GetAimPunchAngle() * 2.f));
			for (int t = 0; t < gVars.nBacktrackTicks; ++t)
			{
				float flTempFOVDistance = gMath.DistancePointToLine(Data[nBestTargetIndex][t].m_vecHitboxPosition, pLocal->GetEyePosition(), ViewDir);
				if (flTempFloat > flTempFOVDistance && Data[nBestTargetIndex][t].m_flSimulationTime > pLocal->GetSimulationTime() - 1)
				{
					flTempFloat = flTempFOVDistance;
					flBestTargetSimTime = Data[nBestTargetIndex][t].m_flSimulationTime;
				}
			}

			if (cmd->buttons & IN_ATTACK && flBestTargetSimTime > 0)
			{
				cmd->tick_count = TIME_TO_TICKS(flBestTargetSimTime);
			}
		}
	}
}

void AutoJump(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	if (!gVars.bBunnyhop)
		return;

	static bool bJumpedLastTick = false;
	static bool bShouldFakeJump = false;

	if (!bJumpedLastTick && bShouldFakeJump)
	{
		bShouldFakeJump = false;
		pCommand->buttons |= IN_JUMP;
	}
	else if (pCommand->buttons & IN_JUMP)
	{
		if (pLocal->GetFlags() & FL_ONGROUND)
		{
			bJumpedLastTick = true;
			bShouldFakeJump = true;
		}
		else
		{
			pCommand->buttons &= ~IN_JUMP;
			bJumpedLastTick = false;
		}
	}
	else
	{
		bJumpedLastTick = false;
		bShouldFakeJump = false;
	}
}


bool TraceToPlayer(CBaseEntity* pLocal, CUserCmd* pCommand, CBaseEntity* pBaseEntity)
{
	if (!GetAsyncKeyState(VK_XBUTTON2))
		return false;

	Ray_t ray;
	trace_t trace;
	CTraceFilter filter;

	Vector vecLocalViewAngles;
	gInts.Engine->GetViewAngles(vecLocalViewAngles);

	Vector vecRange;
	AngleVectors(vecLocalViewAngles, &vecRange);

	vecRange = vecRange * 8192 + pLocal->GetEyePosition();

	filter.pSkip1 = pLocal;

	ray.Init(pLocal->GetEyePosition(), vecRange);

	gInts.EngineTrace->TraceRay(ray, 0x46004003, &filter, &trace);

	if (trace.m_pEnt == NULL)
		return false;

	if (trace.m_pEnt->IsDormant())
		return false;

	if (trace.m_pEnt->GetTeam() == pLocal->GetTeam())
		return false;

	if (trace.m_pEnt->IsAlive() == false)
		return false;

	if (trace.m_pEnt->GetClientClass()->iClassID != 40)
		return false;

	if (trace.hitbox < 0 || trace.hitbox > 7)
		return false;

	return true;
}

//tracebot reminds me of russians calling backtracking "backshoot"
void RunTraceBot(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	if (!gVars.bTriggerbot)
		return;

	if (!pLocal)
		return;

	if (!pLocal->IsAlive())
		return;

	for (int i = 1; i <= gInts.Globals->maxClients; i++)
	{
		CBaseEntity* pPlayer = GetBaseEntity(i);

		if (!pPlayer || pPlayer->IsDormant() || pPlayer->GetTeam() == pLocal->GetTeam() || pPlayer == pLocal)
			continue;

		if (TraceToPlayer(pLocal, pCommand, pPlayer))
			pCommand->buttons |= IN_ATTACK;
	}
}

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand)
{
	bReturn(ClientMode, input_sample_frametime, pCommand);

	CBaseEntity* pBaseEntity = GetBaseEntity(me);

	if (pBaseEntity == NULL)
		return bReturn;
	
	if (!pCommand || !pCommand->command_number)
		return bReturn;

	if (bReturn)
	    gInts.Engine->SetViewAngles(pCommand->viewangles);

	uintptr_t _BP;
	__asm mov _BP, ebp;
	gVars.bSendPacket = (bool*)(***(uintptr_t***)_BP - 1);

	AutoJump(pBaseEntity, pCommand);

	gAimbot.Think(pCommand, pBaseEntity);

	BacktrackPlayer(pCommand, pBaseEntity);

	RunTraceBot(pBaseEntity, pCommand);

	return false;
}

void FullUpdate()
{
	typedef void(*ForceUpdate) (void);
	ForceUpdate FullUpdate = (ForceUpdate)gSignatures.GetEngineSignature("A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85");
	FullUpdate();
}

void GloveChanger(CBaseEntity* pLocal)
{
	if (!gVars.bSkinChanger)
		return;

	if (!gInts.Engine->IsInGame())
		return;

	if (!pLocal || !pLocal->IsAlive())
		return;

	auto pWearables = pLocal->GetWearables();

	if (!gInts.EntList->GetClientEntityFromHandle(pWearables[0]))
	{
		for (ClientClass* pClass = gInts.Client->GetAllClasses(); pClass; pClass = pClass->pNextClass)
		{
			if (pClass->iClassID == 54)
			{
				int iEntry = gInts.EntList->GetHighestEntityIndex() + 1;
				int iSerial = rand() % 0x1000;

				if (pClass->m_pCreateFn(iEntry, iSerial))
				{
					pWearables[0] = iEntry | (iSerial << 16);
					break;
				}
			}
		}

		if (!pWearables[0])
			return;

		CGloves* pGloves = (CGloves*)gInts.EntList->GetClientEntityFromHandle(pWearables[0]);

		if (!pGloves)
			return;

		player_info_t info;
		if (!gInts.Engine->GetPlayerInfo(gInts.Engine->GetLocalPlayer(), &info))
			return;

		/*if (pGloves->GetItemDefinitionIndex() != GLOVE_SPECIALIST || *pGloves->GetFallbackPaintKit() != 10033) //Crimson Kimono
		{
			pGloves->SetGloveModelIndex(gInts.ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
			pGloves->SetItemDefinitionIndex(GLOVE_SPECIALIST);
			*pGloves->GetFallbackPaintKit() = 10033;
			*pGloves->GetEntityQuality() = 4;
			*pGloves->GetFallbackSeed() = 0;
			*pGloves->GetFallbackWear() = std::numeric_limits<float>::min();
			*pGloves->GetFallbackStatTrak() = -1;
			*pGloves->GetItemIDHigh() = -1;
			*pGloves->GetAccountID() = info.xuidlow;
			*reinterpret_cast<int*>(uintptr_t(pGloves) + 0x64) = -1;
			pGloves->PreDataUpdate(0);
		}*/

		//GLOVE_SPORTY
		if (pGloves->GetItemDefinitionIndex() != GLOVE_LEATHER_WRAP || *pGloves->GetFallbackPaintKit() != 10054)
		{
			pGloves->SetGloveModelIndex(gInts.ModelInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
			pGloves->SetItemDefinitionIndex(GLOVE_LEATHER_WRAP);
			*pGloves->GetFallbackPaintKit() = 10054;
			*pGloves->GetEntityQuality() = 4;
			*pGloves->GetFallbackSeed() = 0;
			*pGloves->GetFallbackWear() = FLT_MIN;
			*pGloves->GetFallbackStatTrak() = -1;
			*pGloves->GetItemIDHigh() = -1;
			*pGloves->GetAccountID() = info.xuidlow;
			*reinterpret_cast<int*>(uintptr_t(pGloves) + 0x64) = -1;
			pGloves->PreDataUpdate(0);
		}
	}

}


void SkinChanger(CBaseEntity* pLocal)
{
	if (!gVars.bSkinChanger)
		return;

	if (!gInts.Engine->IsInGame())
		return;

	if (!pLocal)
		return;

	if (!pLocal->IsAlive())
		return;

	auto hWeapons = pLocal->GetWeapons();

	if (!hWeapons)
		return;

	if (GetAsyncKeyState(VK_F1) & 1)
		FullUpdate();
	

	for (size_t i = 0; hWeapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)gInts.EntList->GetClientEntityFromHandle(hWeapons[i]);

		if (!pWeapon)
		{
			continue;
		}
		

		if (pWeapon->IsKnife(pWeapon->GetItemDefinitionIndex()))
		{
			pWeapon->SetItemDefinitionIndex(WEAPON_KNIFE_M9_BAYONET);
			pWeapon->SetModelIndex(gInts.ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl"));
			*pWeapon->GetFallbackPaintKit() = 417;
			*pWeapon->GetEntityQuality() = 3; //adds the little star :)
		}

		switch (pWeapon->GetItemDefinitionIndex())
		{
		case WEAPON_AWP:
			*pWeapon->GetFallbackPaintKit() = 174; break;
		case WEAPON_AK47:
			*pWeapon->GetFallbackPaintKit() = 801;
			//*pWeapon->GetFallbackSeed() = 151; 
			break;  
		case WEAPON_M4A1:
			*pWeapon->GetFallbackPaintKit() = 255; break; 
		case WEAPON_DEAGLE:
			*pWeapon->GetFallbackPaintKit() = 351; break; 
		case WEAPON_GLOCK:
			*pWeapon->GetFallbackPaintKit() = 48; break; 
		case WEAPON_USP_SILENCER:
			*pWeapon->GetFallbackPaintKit() = 504; break; 
		}

		*pWeapon->GetItemIDHigh() = -1;
	}
}

void __fastcall Hooked_FrameStageNotify(PVOID ClientMode, int edx, ClientFrameStage_t Frame)
{
	CBaseEntity* pLocal = GetBaseEntity(me);

	if (pLocal)
	{
		switch (Frame)
		{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		{
			SkinChanger(pLocal);
			GloveChanger(pLocal);
		} break;
		case FRAME_NET_UPDATE_END:
		{
			for (int i = 1; i <= gInts.Globals->maxClients; i++)
			{
				CBaseEntity* pEntity = GetBaseEntity(i);

				if (!pEntity || pEntity == pLocal || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->GetTeam() == pLocal->GetTeam())
					continue;


				Vector vBackup = pEntity->GetAbsOrigin();

				pEntity->SetAbsOrigin(pEntity->GetOrigin()); // set to uninterpolated data.

				if (!pEntity->SetupBones(gAimbot.m_Matrix[i], 128, 0x100, 0))
					continue;

				//gAim.StoreRecord(pLocal, i);

				pEntity->SetAbsOrigin(vBackup); //restore to interpolated data.

			}
		}
		break;
		}
	}

	oFrameStage(ClientMode, Frame);
}

float __stdcall Hooked_GetViewmodelFov()
{
	if (gInts.Engine->IsInGame() && gInts.Engine->IsConnected())
	{
		CBaseEntity* pLocal = GetBaseEntity(me);

		if (pLocal)
		{
			return gVars.iVMFov;
		}
	}
}

/*void __stdcall Hooked_OverrideView(CViewSetup* pSetup)
{
	oView(pSetup);

	if (gInts.Engine->IsInGame() && gInts.Engine->IsConnected())
	{
		CBaseEntity* pLocal = GetBaseEntity(me);

		if (pLocal)
		{
			if (!pLocal->IsScoped())
				pSetup->fov = gVars.iFov;
		}
	}
}*/
