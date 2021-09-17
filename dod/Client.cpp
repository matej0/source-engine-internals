#include "SDK.h"
#include "Client.h"
#include "CDrawManager.h"
#include <intrin.h>

//"I don't see anything bad in pasting something you completely understand" - v952
unsigned int GetR(void* class_, unsigned int index) { return (unsigned int)(*(int**)class_)[index]; }

Data LagCompensationData[64][12];
RequiredMath gMath;
SendDatagramFn oSendDatagram = nullptr;

void SetInterpolationFlags(CBaseEntity* pEntity, int flag)
{
	auto VarMap = reinterpret_cast<uintptr_t>(pEntity) + 20; // csgo = 36
	auto VarMapSize = *reinterpret_cast<int*>(VarMap + 20);

	for (auto index = 0; index < VarMapSize; index++)
		*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(VarMap) + index * 12) = flag;
}


//https://github.com/lua9520/source-engine-2018-hl2_src/blob/3bf9df6b2785fa6d951086978a3e66f49427166a/game/client/viewrender.cpp#L4569
void __fastcall Hooked_EnableWorldFog(PVOID ecx, PVOID edx)
{ 
	//static DWORD dwLocation = gSignatures.GetClientSignature("8B F0 85 F6 74 07 8B 06 8B CE FF 50 08 33 DB") + 0x2;
	//*reinterpret_cast<IMatRenderContext**>(dwLocation); // mov esi, eax

	//this is a better solution because just returning causes weird visual artifacts on some maps.
	IMatRenderContext* pRenderContext = gInts.MaterialSystem->GetRenderContext();

	if (!pRenderContext)
		oWorldFog(ecx);

	pRenderContext->FogMode(MATERIAL_FOG_NONE);
}

void __stdcall Hooked_EnableSkyBoxFog()
{
	return;
}

void __stdcall Hooked_DoEnginePostProcessing()
{
	//"BackBuffer"
	return;
}

float __fastcall Hooked_GetViewModelFOV(PVOID ecx, PVOID edx)
{
	return 75.f;
}

bool __fastcall Hooked_ShouldDrawFog(PVOID ecx, PVOID edx)
{
	if ((DWORD)_ReturnAddress() != gPlayerVars.dwFogReturnAddress)
	{
		return oFog(ecx);
	}

	return false;
}

void __stdcall Hooked_LevelInitPreEntity(const char* map)
{
	//get server tickrate.
	float flTickrate = 1.f / gInts.Globals->interval_per_tick;

	//set cvars to optimal values.
	gConvars.cl_interp->SetValue(0.0f);
	gConvars.cl_interp_ratio->SetValue(0.0f);
	gConvars.cl_cmdrate->SetValue(flTickrate);
	gConvars.cl_updaterate->SetValue(flTickrate);

	oLevelInit(map);
}

#define NET_FRAMES_BACKUP 64 // must be power of 2. 
#define NET_FRAMES_MASK ( NET_FRAMES_BACKUP - 1 )
int __fastcall Hooked_SendDatagram(CNetChannel* ecx, PVOID edx, PVOID data)
{
	int backup2 = ecx->m_nOutReliableState;
	int backup = ecx->m_nInSequenceNr;
	
	int ret = oSendDatagram(ecx, data);

	ecx->m_nOutReliableState = backup2;
	ecx->m_nInSequenceNr = backup;
	
	return ret;
}

void __stdcall Hooked_LevelInitPostEntity()
{
	gPlayerVars.pNetChannel = gInts.Engine->GetNetChannelInfo();
	PVOID pTarget;

	if (gPlayerVars.pNetChannel)
	{
		//couldnt figure out how to do it with minhook
		pTarget = reinterpret_cast<PVOID>(GetR(gPlayerVars.pNetChannel, 46));
		MH_DisableHook(pTarget);
		MH_CreateHook(pTarget, &Hooked_SendDatagram, reinterpret_cast<void**>(&oSendDatagram));
	}

	oLevelPost();
}


void RemoveCommandLimitations()
{
	//"Pay close attention to the second link - we cannot remove the client-sided limit on new commands. 
	//If we do so, the number of commands written will not match the number read, and the server will kick you (see the final link). 
	//This means that we are unable to write additional commands as we can in CS:GO since protobufs are used." - ucuser0

	//there go my dreams of 22 tick fakelag.

	auto clMoveChokeClamp = gSignatures.GetEngineSignature("B9 ? ? ? ? 3B C1 0F 4F C1 8B F7") + 1;
	unsigned long protect = 0;
	VirtualProtect((void*)clMoveChokeClamp, 4, PAGE_EXECUTE_READWRITE, &protect);
	*(std::uint32_t*)clMoveChokeClamp = 62;
	VirtualProtect((void*)clMoveChokeClamp, 4, protect, &protect);
}




void SimulateLag(CUserCmd* pCommand)
{
	if (GetAsyncKeyState(VK_XBUTTON1) || GetAsyncKeyState(VK_MBUTTON))
	{
		static int iChokedCommands;
		int iMaxChokedCommands = 14;

		static int iTickTimer;
		static int iFakeLag = iTickTimer + gPlayerVars.iFakeLagAmount;

		if (iFakeLag >= iTickTimer)
		{
			*gPlayerVars.bSendPacket = false;
			iChokedCommands++;
		}
		else
			iFakeLag = iTickTimer + gPlayerVars.iFakeLagAmount;

		iTickTimer++;

		if (iChokedCommands > iMaxChokedCommands)
			*gPlayerVars.bSendPacket = true;

		if (gPlayerVars.bSendPacket)
			iChokedCommands = 0;
	}
}

void ApplyBulletSpread(CUserCmd* cmd, CBaseEntity* pLocal, CBaseCombatWeapon* w, Vector &ref, float m)
{
	if (!pLocal || !w)
		return;

	//couldnt be bothered as this is only place where i need em
	static void(*RandomSeed)(unsigned) = (void(__cdecl*)(unsigned))IMPORT("vstdlib", "RandomSeed");
	static float(*RandomFloat)(float, float) = (float(__cdecl*)(float, float))IMPORT("vstdlib", "RandomFloat");

	cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & 255;

	Vector shake = Vector();

	float(__thiscall* GetBulletSpread)(void*, float) = getvfunc<float(__thiscall*)(void*, float)>(w, 373);
	float f = GetBulletSpread(w, pLocal->GetVelocity().Length2D());

	shake.x = f;
	shake.y = f;

	if (cmd->random_seed == 39)
		cmd->random_seed += 2;

	RandomSeed(cmd->random_seed);

	float x = RandomFloat(-0.5f, 0.5f) + RandomFloat(-0.5f, 0.5f);
	float y = RandomFloat(-0.5f, 0.5f) + RandomFloat(-0.5f, 0.5f);

	Vector forward, right, up;

	NormalizeAngles(cmd->viewangles);
	AngleVectors(cmd->viewangles, &forward, &right, &up);

	Vector new_va = forward + (right * shake.x * x * m) + (up * shake.y * y * m);
	new_va.NormalizeInPlace();

	VectorAngles(new_va, ref);
	NormalizeAngles(ref);
}

bool WantsLagCompensation(CBaseEntity* pEntity, CBaseEntity* pLocal)
{
	return (pEntity && pEntity->IsAlive() && !pEntity->IsDormant() && pEntity->GetTeam() != pLocal->GetTeam() && pEntity != pLocal);
}

void BacktrackPlayer(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	if (!pLocal || !pLocal->IsAlive())
		return;

	int iBestTargetIndex = -1;
	float flBestFov = FLT_MAX;
	player_info_t pInfo;


	for (int i = 1; i < gInts.Engine->GetMaxClients(); i++)
	{
		CBaseEntity* pBaseEntity = GetBaseEntity(i);

		if (!WantsLagCompensation(pBaseEntity, pLocal))
			continue;

		if (!gInts.Engine->GetPlayerInfo(i, &pInfo))
			continue;

		float flSimulationTime = pBaseEntity->GetSimulationTime();

		Vector vHitboxHead = pBaseEntity->GetHitboxPosition(14);

		LagCompensationData[i][pCommand->command_number % 12] = Data{ flSimulationTime, vHitboxHead };

		pBaseEntity->SetupBones(LagCompensationData[i][pCommand->command_number % 12].Matrix, 128, 256, gInts.Globals->curtime);

		Vector ViewDir;
		gMath.AngleVectors(pCommand->viewangles, ViewDir);

		float FOVDistance = gMath.DistancePointToLine(vHitboxHead, pLocal->GetEyePosition(), ViewDir);

		if (flBestFov > FOVDistance)
		{
			flBestFov = FOVDistance;
			iBestTargetIndex = i;
		}
	}

	float flBestTargetSimTime;
	if (iBestTargetIndex != -1)
	{
		float flTempFloat = FLT_MAX;
		Vector ViewDir;
		gMath.AngleVectors(pCommand->viewangles, ViewDir);

		for (int t = 0; t < 12; ++t)
		{
			float flTempFOVDistance = gMath.DistancePointToLine(LagCompensationData[iBestTargetIndex][t].vecHead, pLocal->GetEyePosition(), ViewDir);
			if (flTempFloat > flTempFOVDistance && LagCompensationData[iBestTargetIndex][t].flSimulationTime > pLocal->GetSimulationTime() - 1)
			{
				flTempFloat = flTempFOVDistance;
				flBestTargetSimTime = LagCompensationData[iBestTargetIndex][t].flSimulationTime;
			}
		}

		if (pCommand->buttons & IN_ATTACK)
		{
			pCommand->tick_count = TIME_TO_TICKS(flBestTargetSimTime);
		}
	}

}

void CantTouchThis(CUserCmd* pCommand, CBaseEntity* pLocal)
{
	if (!gPlayerVars.bGodMode)
		return;

	if (!pLocal || !pLocal->IsAlive())
		return;

	if (pCommand->buttons & IN_ATTACK)
		return;

	//if (!GetAsyncKeyState(VK_XBUTTON1))
	//*gPlayerVars.bSendPacket = !gPlayerVars.bSendPacket;

	if (!gPlayerVars.bSendPacket)
		pCommand->viewangles.y += 60.f;
}

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand)
{
	CBaseEntity* pBaseEntity = GetBaseEntity(me);

	if (pBaseEntity == NULL)
		return bReturn;

	if (!pCommand || !pCommand->command_number)
		return bReturn;

	gInts.Engine->SetViewAngles(pCommand->viewangles);

	uintptr_t _BP;
	__asm mov _BP, ebp;
	gPlayerVars.bSendPacket = (bool*)(***(uintptr_t***)_BP - 1);

	Vector vecOldViewAngles = pCommand->viewangles;
	float flOldForwardMove = pCommand->forwardmove;
	float flOldSideMove = pCommand->sidemove;

	SimulateLag(pCommand);

	BacktrackPlayer(pBaseEntity, pCommand);

	if (gPlayerVars.bNoSpread && GetAsyncKeyState(VK_LBUTTON))
	{
		if (pBaseEntity->GetActiveWeapon())
	    {
			//you cant shoot while jumping and no point in nospreading with nade
			if (!pBaseEntity->GetActiveWeapon()->IsHoldingNade() && pBaseEntity->GetFlags() & FL_ONGROUND)
			{
				Vector vOldView = pCommand->viewangles;
				float flOldSidemove = pCommand->sidemove;
				float flOldForwardmove = pCommand->forwardmove;

				float flServerTime = pBaseEntity->GetTickBase() * gInts.Globals->interval_per_tick;
				float flNextPrimaryAttack = pBaseEntity->GetActiveWeapon()->GetNextPrimaryAttack();

				bool bBulletTime = true;
				if (flNextPrimaryAttack > flServerTime)
					bBulletTime = false;

				if (pCommand->buttons & IN_ATTACK && bBulletTime)
				{
					*gPlayerVars.bSendPacket = false;
					ApplyBulletSpread(pCommand, pBaseEntity, pBaseEntity->GetActiveWeapon(), pCommand->viewangles, -1.0f);
				}
				else
				{
					*gPlayerVars.bSendPacket = true;
					pCommand->viewangles = vOldView;
					pCommand->sidemove = flOldSidemove;
					pCommand->forwardmove = flOldForwardmove;
				}
			}
		}
	}

	/*if (pBaseEntity->IsAlive() && GetAsyncKeyState(VK_XBUTTON2))
	{
		CNetChannel* pNetChannel = gInts.Engine->GetNetChannelInfo();
		pNetChannel->m_nOutSequenceNr += 700;
	}*/

	//gMath.CorrectMovement(vecOldViewAngles, pCommand, flOldForwardMove, flOldSideMove);
	bReturn(ClientMode, input_sample_frametime, pCommand);

	return false;
}

void __fastcall Hooked_FrameStageNotify(PVOID ClientMode, int edx, ClientFrameStage_t Frame)
{
	oFrameStage(ClientMode, Frame);

	CBaseEntity* pLocal = GetBaseEntity(me);

	if (pLocal != NULL && gInts.Engine->IsInGame() && gInts.Engine->IsConnected())
	{
		switch (Frame)
		{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		{
			for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
			{
				CBaseEntity* pEntity = GetBaseEntity(i);

				if (!pEntity || pEntity == pLocal || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->GetTeam() == pLocal->GetTeam())
					continue;

				SetInterpolationFlags(pEntity, DISABLE_INTERPOLATION);
			}
		}
		break;
		}
	}
}

void __stdcall Hooked_OverrideView(CViewSetup* pSetup)
{
	oView(pSetup);

	if (gInts.Engine->IsInGame() && gInts.Engine->IsConnected())
	{
		CBaseEntity* pLocal = GetBaseEntity(me);

		if (pLocal )
		{
			//this lowers fov when u zoom but theres a slight delay until ur actually scoped. tried rebuilding what the game does but didnt work.
			//also triggers if u hold melee XD - not anymore.
			if (!pLocal->GetActiveWeapon()->IsZoomed())
			    pSetup->m_fov = gPlayerVars.iFov;
		}
	}
}
