#include "SDK.h"
#include "Client.h"
#include "CDrawManager.h"
#include <intrin.h>
#include "Aimbot.h"
#include "Misc.h"
//"I don't see anything bad in pasting something you completely understand" - v952

bool __fastcall Hooked_ShouldDrawFog(PVOID ecx, PVOID edx)
{
	if ((DWORD)_ReturnAddress() != gPlayerVars.dwFogReturnAddress)
	{
		return oFog(ecx);
	}

	return false;
}

//tf2 varmap ent + 20

void __fastcall Hooked_MaintainSequenceTransitions(PVOID ecx, PVOID edx, IBoneSetup &boneSetup, float flCycle, Vector pos[], Quaternion q[])
{
	//called on client during bone setup but not on server, so off it goes.
	gPlayerVars.Debug.msqt = true;
	return;
}

bool __fastcall Hooked_IsPlayingBack(PVOID ecx, int edx)
{
	if ((DWORD)_ReturnAddress() != gPlayerVars.dwPureReturnAddress)
	{
		return oIsPlayingBack(ecx);
	}

	return true;
}

bool g_bConstructingBones;
bool ConstructBones(CBaseEntity* pEntity, matrix3x4* pBones, int mask)
{
	//credits to ucuser0, i followed his thread. i left out some animation related stuff which i deemeed unnecessary? maybe i do need to add it.
	//does this shit even do anything? no fuckin clue.
	if (!pEntity || !gPlayerVars.bAimbot)
		return false;

	Vector m_vecUninterpolatedOrigin = pEntity->GetOrigin();
	Vector m_vecInterpolatedOrigin   = pEntity->GetAbsOrigin();
	Vector m_vecAbsAngles            = pEntity->GetAbsAngles();
	Vector m_vecNetworkedAngles      = pEntity->GetEyeAngles();

	//static ConVar* anim_showstate = gInts.Cvar->FindVar("anim_showstate");
	//anim_showstate->SetValue(pEntity->GetIndex());

	g_bConstructingBones = true;
	
	pEntity->SetAbsAngles(m_vecNetworkedAngles);
	pEntity->SetAbsOrigin(m_vecUninterpolatedOrigin);
	pEntity->AddFlag(EFL_DIRTY_ABSTRANSFORM | EFL_DIRTY_ABSVELOCITY);
	pEntity->CalcAbsolutePosition(); //recalculates m_vecAbsOrigin? do i need this? idk.

	if (!pEntity->SetupBones(pBones, 128, mask, gInts.Globals->curtime))
		return false;

	//pEntity->SetAbsOrigin(m_vecInterpolatedOrigin);
	//pEntity->SetAbsAngles(m_vecAbsAngles);

	g_bConstructingBones = false;

	return true;
}

void __fastcall Hooked_AddFlag(PVOID ecx, PVOID edx, int flag)
{
	//prevents EFL_DIRTY_ABSTRANSFORM from being set?
	if (g_bConstructingBones && (flag & EFL_SETTING_UP_BONES))
	{
		gPlayerVars.Debug.addflag = true;
		return;
	}

	oAddFlag(ecx, flag);
}

bool __fastcall Hooked_Teleported(PVOID ecx, PVOID edx)
{
	if (g_bConstructingBones)
	{
		gPlayerVars.Debug.teleported = true;
		return true;
	}

	return oTeleported(ecx);
}

void __fastcall Hooked_UpdateClientSideAnimations(CBaseEntity* ecx, PVOID edx)
{
	if (gPlayerVars.m_bShouldUpdateAnimations)
		oUpdateClientSideAnimations(ecx);
}

void __fastcall Hooked_EstimateAbsVelocity(CBaseEntity* thisptr, PVOID edx, Vector &vel)
{
	if (gPlayerVars.m_bShouldUpdateAnimations)
	{
		vel = thisptr->GetAbsVelocity();
		gPlayerVars.Debug.velocity = true;
		return;
	}
		
	
	//if (!thisptr || !thisptr->IsAlive())
	oEstimateAbsVelocity(thisptr, vel);
}

void __fastcall Hooked_GetOuterAbsVelocity(PVOID ecx, PVOID edx, Vector &vel)
{
	if (ecx == nullptr)
	{
		oGetOuterAbsVelocity(ecx, vel);
		return;
	}


	CBaseEntity* pEntity = *reinterpret_cast<CBaseEntity**>(ecx);
	
	if (pEntity)
	{
		if (!pEntity->IsOnGround())
		{
			oGetOuterAbsVelocity(ecx, vel);
		}
	}

	//oGetOuterAbsVelocity(ecx, vel);
}

void __fastcall Hooked_UpdatePlayerList(PVOID ecx, PVOID edx)
{
	CBaseEntity* pLocal = GetBaseEntity(me);

	if (pLocal)
	{
		//AreEnemyTeams
		//i tried setting to other team which didnt work.
		//static int m_iTeam = gNetvars->GetOffset("DT_BaseEntity", "m_iTeamNum");
		//*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(pLocal) + m_iTeam) = 1; //TEAM_SPECTATOR
	}
	

	oUpdatePlayerList(ecx);
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


void __fastcall Hooked_ItemPostFrame(CBaseEntity* ecx, PVOID edx)
{
	//i dont feel like rebuilding postthink.
	if (gPlayerVars.m_bSelfPredicting)
			return;
	
	oItemPostFrame(ecx);
}

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand)
{
	CBaseEntity* pBaseEntity = GetBaseEntity(me);

	if (pBaseEntity == NULL || !pCommand || !pCommand->command_number)
		return bReturn;

	gInts.Engine->SetViewAngles(pCommand->viewangles);

	uintptr_t _BP;
	__asm mov _BP, ebp;
	gPlayerVars.bSendPacket = (bool*)(***(uintptr_t***)_BP - 1);

	Vector vecOldViewAngles = pCommand->viewangles;
	float flOldForwardMove = pCommand->forwardmove;
	float flOldSideMove = pCommand->sidemove;

	gMisc.SimulateLag(pCommand);
	gMisc.RemoveDisguise();
	gMisc.AutoJump(pBaseEntity, pCommand);
	gAim.AutoBackstab(pBaseEntity, pCommand);

	//gAim.BacktrackPlayer(pBaseEntity, pCommand);

	gEnginePred.Start(pBaseEntity, pCommand);

	gAim.Think(pBaseEntity, pCommand);

	gEnginePred.End(pBaseEntity);

	gMath.CorrectMovement(vecOldViewAngles, pCommand, flOldForwardMove, flOldSideMove);

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
		case FRAME_NET_UPDATE_END:
		{

			for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
			{
				CBaseEntity* pEntity = GetBaseEntity(i);

				if (!pEntity || pEntity == pLocal || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->GetTeam() == pLocal->GetTeam())
					continue;

			/*	Vector vecOldOrigin = pEntity->GetOrigin();

				pEntity->SetAbsOrigin(pEntity->GetOrigin());

				pEntity->SetupBones(gAim.m_Matrix[i], 128, 0x100, gInts.Globals->curtime);

				gAim.StoreRecord(pLocal, i);

				pEntity->SetAbsOrigin(vecOldOrigin);*/

				if (!ConstructBones(pEntity, gAim.m_Matrix[i], 256))
					continue;

				gAim.StoreRecord(pLocal, i);
			}
		}
		break;
		case FRAME_RENDER_START:
		{
			//gPlayerVars.m_bShouldUpdateAnimations = false;
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

		if (pLocal)
		{
			if (!(pLocal->GetCond() & TFCond_Zoomed))
			    pSetup->m_fov = gPlayerVars.iFov;
		}
	}
}

void __fastcall Hooked_RunCommand(CPrediction *pPrediction, PVOID edx, CBaseEntity *player, CUserCmd *ucmd, IMoveHelper *moveHelper)
{
	if (player && player->GetIndex() == me && gPlayerVars.m_bSelfPredicting) //dont run games prediction while doing our own.
		return;

	oRunCommand(pPrediction, player, ucmd, moveHelper);
}
