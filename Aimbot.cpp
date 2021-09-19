#include "Aimbot.h"
CAimbot gAim;
CEnginePrediction gEnginePred;
ISort gTargetSort;

Vector GetClosestHitboxToCrosshair(CBaseEntity* pLocal, CBaseEntity* pEntity, matrix3x4* matrix, Vector vecViewAngles)
{
	Vector                  vecHitbox;
	mstudiobbox_t*          hitbox;
	mstudiohitboxset_t*     set;
	studiohdr_t*            hdr;

	float MaxFOV = 360.f;

	hdr = gInts.ModelInfo->GetStudiomodel(pEntity->GetModel());

	if (!hdr)
		return Vector(0, 0, 0);

	set = hdr->GetHitboxSet(0);

	if (!set)
		return Vector(0, 0, 0);

	for (int i = 0; i < 7; i++)
	{
		hitbox = set->pHitbox(i);

		if (!hitbox)
			continue;

		float FOV = gMath.GetFOV(vecViewAngles, pLocal->GetEyePosition(), pEntity->GetHitboxPositionCustom(i, matrix));

		if (FOV < MaxFOV)
		{
			MaxFOV = FOV;
			vecHitbox = pEntity->GetHitboxPositionCustom(i, matrix);
			continue;
		}
	}

	return vecHitbox;
}

void CAimbot::Init() //i intended to do some other shit here but whatevz.
{
	gAim.m_Targets.clear();
}

void Predict(CBaseEntity* pLocal, CBaseEntity* pEntity, Vector& vecHitbox)
{
	float flGravityModifier = 1.f;
	float flTimeToImpact    = 0.0f;
	float flGravity         = 0.0f;
	float flProjectileSpeed = 0.0f;

	Vector vecVelocity = Vector();
	pEntity->EstimateAbsVelocity(vecVelocity);

	bool bIsHuntsman = (pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_Huntsman || pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_CompoundBow || pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_FestiveHuntsman);

	if (bIsHuntsman)
	{
		//aim for head with bow on ground, in air aim for center of mass. maximizes chances of hitting.
		if (pEntity->GetFlags() & FL_ONGROUND)
			vecHitbox = pEntity->GetHitboxPositionCustom(HITBOX_HEAD, gAim.m_Matrix[pEntity->GetIndex()]);
		else
			vecHitbox = pEntity->GetHitboxPositionCustom(3, gAim.m_Matrix[pEntity->GetIndex()]);

		flGravityModifier = pLocal->GetActiveWeapon()->GetArrowGravity();
	}
	else
	{
		if (pLocal->GetClassNum() == TF2_Soldier || !(pEntity->GetFlags() & FL_ONGROUND))
			vecHitbox = pEntity->GetOrigin();
		else
			vecHitbox = pEntity->GetHitboxPositionCustom(3, gAim.m_Matrix[pEntity->GetIndex()]); //aim at the center otherwise 
	}

	flProjectileSpeed = pLocal->GetActiveWeapon()->GetProjectileSpeed();
	flTimeToImpact    = pLocal->GetEyePosition().DistTo(vecHitbox) / flProjectileSpeed;
	flGravity         = (0.5f * 800.f * flGravityModifier) * std::powf(flTimeToImpact, 2);

	vecHitbox.x += (vecVelocity.x * flTimeToImpact);
	vecHitbox.y += (vecVelocity.y * flTimeToImpact);

	if (!(pEntity->GetFlags() & FL_ONGROUND))
		vecHitbox.z += -flGravity + ((vecVelocity.z * flTimeToImpact));
	else if (bIsHuntsman)
		vecHitbox.z += flGravity + (vecVelocity.z * flTimeToImpact);
}

Vector CAimbot::GetBestHitbox(CBaseEntity* pLocal, CBaseEntity* pEntity, CUserCmd* pCmd, matrix3x4* pMatrix)
{
	Vector vecHitbox = Vector();

	if (pLocal->IsHoldingProjectileWeapon())
	{
		Predict(pLocal, pEntity, vecHitbox);
	}
	else
	{
		if (gPlayerVars.bHitscan)
		{
			vecHitbox = GetClosestHitboxToCrosshair(pLocal, pEntity, pMatrix, pCmd->viewangles);
		}
		else
		{
			bool bIsSniper = (pLocal->GetCond() & TFCond_Zoomed && pLocal->GetClassNum() == TF2_Sniper && pLocal->GetActiveWeapon()->GetSlot() == TF_WEAPONSLOT_PRIMARY);
			bool bIsAmbySpy = (pLocal->GetClassNum() == TF2_Spy && pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_Ambassador);
			
			if (bIsSniper || bIsAmbySpy)
				vecHitbox = pEntity->GetHitboxPositionCustom(HITBOX_HEAD, pMatrix);
			else
				vecHitbox = pEntity->GetHitboxPositionCustom(HITBOX_PELVIS, pMatrix);
		}
	}

	if (vecHitbox.IsZero() || !UTIL_IsEntityVisible(pLocal, pEntity, pLocal->GetEyePosition(), vecHitbox))
		return Vector(0, 0, 0);

	return vecHitbox;
}

void CAimbot::FindTargets(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		if (i == me)
			continue;

		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->GetTeam() == pLocal->GetTeam())
			continue;

		if (gPlayerVars.bAimIgnores[CLOAKED] && pEntity->GetCond() & TFCond_Cloaked)
			continue;

		if (gPlayerVars.bAimIgnores[UBERED] && (pEntity->GetCond() & TFCond_Ubercharged || pEntity->GetCond() & TFCond_UberchargeFading))
			continue;

		if (gPlayerVars.bAimIgnores[DISGUISED] && pEntity->GetCond() & TFCond_Disguised)
			continue;

		if (gPlayerVars.bAimIgnores[TAUNTING] && pEntity->GetCond() & TFCond_Taunting)
			continue;

		if (gPlayerVars.bAimIgnores[BONKED] && pEntity->GetCond() & TFCond_Bonked)
			continue;

		float flFOV = gMath.GetFOV(pCommand->viewangles, pLocal->GetEyePosition(), pEntity->GetEyePosition());

		if (flFOV > gPlayerVars.iAimbotFov)
			continue;

		//add to the list.
		IPlayerData pData;
		pData.m_Index = i;
		pData.m_pEntity = pEntity;
		pData.m_Fov = flFOV;
		gAim.m_Targets.emplace_back(pData);
	}
}

void CAimbot::Think(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	if (!gPlayerVars.bAimbot)
		return;

	if (!GetAsyncKeyState(VK_XBUTTON2))
		return;

	if (!pLocal->GetActiveWeapon() || !pLocal->CanFireWithCurrentWeapon())
		return;

	Init(); //you NEED to clear it. otherwise it just fills up with garbage.
	gAim.FindTargets(pLocal, pCmd);

	//array empty, dont bother, shouldnt happen but idk.
	if (gAim.m_Targets.empty())
		return;

	CBaseEntity* pBestTarget = nullptr;
	Vector       vecBestHitbox = Vector();
	int          tick_count = -1;

	std::sort(gAim.m_Targets.begin(), gAim.m_Targets.end(), gTargetSort); //sort the fuckers.

	gDebug.m_nTargetListSize = gAim.m_Targets.size();

	for (auto& target : gAim.m_Targets)
	{
		vecBestHitbox = gAim.GetBestHitbox(pLocal, target.m_pEntity, pCmd, gAim.m_Matrix[target.m_Index]);

		if (vecBestHitbox == Vector(0, 0, 0))
		{
			//if we get here, it prolly means the target isnt visible, so lets check if their record is visible.

			if (!pLocal->IsHoldingProjectileWeapon() && gPlayerVars.bBacktracking)
			{
				ILagData* pRecord = gAim.GetLastRecord(target.m_Index);

				if (!pRecord)
					continue;

				vecBestHitbox = gAim.GetBestHitbox(pLocal, target.m_pEntity, pCmd, pRecord->m_pMatrix);

				if (vecBestHitbox == Vector(0, 0, 0))
					continue;

				if (!pRecord->IsRecordOutOfBounds())
					tick_count = TIME_TO_TICKS(pRecord->m_flSimulationTime + UTIL_GetLerpTime());

				pBestTarget = target.m_pEntity; //set the entity, and exit the loop. not really necessary but whatevz.
				break;

			}


			continue;
		}


		pBestTarget = target.m_pEntity;
		break;
	}

	if (!pBestTarget) // no target found wtf.
	{
		gDebug.m_bTargetWasFound = false;
		return;
	}

	gDebug.m_bTargetWasFound = true;

	float  flOldSidemove    = pCmd->sidemove;
	float  flOldForwardmove = pCmd->forwardmove;
	Vector vOldView         = pCmd->viewangles;
	Vector vecStart         = pLocal->GetEyePosition();
	Vector vecTarget        = vecBestHitbox;
	Vector vecAngle         = Vector();

	VectorAngles((vecTarget - vecStart), vecAngle);
	ClampAngle(vecAngle);

	bool bHuntsman = (pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_Huntsman ||
		pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_CompoundBow ||
		pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WPN_FestiveHuntsman
		&& pLocal->GetActiveWeapon()->GetChargeBeginTime() != 0);

	//set aimbot angle.
	pCmd->viewangles = vecAngle;

	if (!gPlayerVars.bSilent)
	    gInts.Engine->SetViewAngles(pCmd->viewangles);

	if (gPlayerVars.bAutoshoot)
	{
		if (bHuntsman && pCmd->buttons & IN_ATTACK)
			pCmd->buttons &= ~IN_ATTACK;
		else
			pCmd->buttons |= IN_ATTACK;
	}

	if (gPlayerVars.bSilent && !bHuntsman)
	{
		if (pCmd->buttons & IN_ATTACK && pLocal->CanFireWithCurrentWeapon())
		{
			*gPlayerVars.bSendPacket = false;
		}
		else
		{
			*gPlayerVars.bSendPacket = true;
			pCmd->viewangles = vOldView;
			pCmd->sidemove = flOldSidemove;
			pCmd->forwardmove = flOldForwardmove;
		}
	}

	if (tick_count == -1) //if tick_count is still -1, means we cant backtrack their last record
		tick_count = TIME_TO_TICKS(pBestTarget->GetSimulationTime() + UTIL_GetLerpTime());

	pCmd->tick_count = tick_count;
}

bool WantsLagCompensation(CBaseEntity* pEntity, CBaseEntity* pLocal)
{
	return (pEntity && pEntity->IsAlive() && !pEntity->IsDormant() && pEntity->GetTeam() != pLocal->GetTeam() && pEntity != pLocal);
}


ILagData* CAimbot::GetLastRecord(int nIndex)
{
	if (gAim.m_Records[nIndex].empty())
		return nullptr;

	return &gAim.m_Records[nIndex].back();
}

bool CAimbot::IsNetworkingUpdate(CBaseEntity* pEntity)
{
	return(pEntity->GetSimulationTime() != pEntity->GetOldSimulationTime());
}

//not very optimized.
void CAimbot::StoreRecord(CBaseEntity* pLocal, int i)
{
	if (!gPlayerVars.bBacktracking)
		return;

	if (!pLocal || !pLocal->IsAlive())
		return;


	ILagData data;
	CBaseEntity* pEntity = GetBaseEntity(i);

	if (!WantsLagCompensation(pEntity, pLocal))
	{
		return;
	}

	if (gAim.m_Records[i].size() > TIME_TO_TICKS(TICK_INTERVAL * 12)) //we dont want more than 12 ticks.
		gAim.m_Records[i].pop_back();

	Vector ViewDir;
	Vector vecViewAngles;
	gInts.Engine->GetViewAngles(vecViewAngles);
	gMath.AngleVectors(vecViewAngles, ViewDir);
	float FOV = gMath.DistancePointToLine(pEntity->GetEyePosition(), pLocal->GetEyePosition(), ViewDir);

	matrix3x4 matrix[128];

	data.m_nIndex           = i;
	data.m_flSimulationTime = pEntity->GetSimulationTime();
	data.m_vecEyePosition   = pEntity->GetEyePosition();
	data.m_vecEyeAngles     = pEntity->GetEyeAngles();
	data.m_vecOrigin        = pEntity->GetOrigin();
	data.m_Fov              = FOV;


	memcpy(data.m_pMatrix, gAim.m_Matrix[i], sizeof(matrix3x4) * 128);

	gAim.m_Records[i].insert(gAim.m_Records[i].begin(), data); //push_front didnt work? maybe emplace_front would? idk. edit: im stupid.
}

bool DoSwingTrace(CBaseEntity* pLocal, CGameTrace* pTrace)
{
	static Vector vecSwingMins(-18, -18, -18);
	static Vector vecSwingMaxs(18, 18, 18);
	float flKnifeRange = 48.f; //48.f for all except sword ( its like 70 normal and 120 when charging ).

	Vector vecForward;
	AngleVectors(pLocal->GetEyePosition(), &vecForward);

	Vector vecSwingStart = pLocal->GetEyePosition();
	Vector vecSwingEnd = vecSwingStart + vecForward * flKnifeRange;

	CTraceFilter filter;
	filter.pSkip = pLocal;
	UTIL_TraceLine(vecSwingStart, vecSwingEnd, MASK_SHOT, &filter, pTrace);

	if (pTrace->fraction >= 1.0f)
	{
		//tf2 does a hull trace if the trace ray doesnt hit anything
		UTIL_TraceHull(vecSwingStart, vecSwingEnd, vecSwingMins, vecSwingMaxs, MASK_SOLID, &filter, pTrace);

		if (pTrace->fraction < 1.0f) //idk if we really need to do this but whatever.
		{
			vecSwingEnd = pTrace->endpos;
		}
	}

	return pTrace->fraction < 1.0f;
}



void CAimbot::AutoBackstab(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	if (!gPlayerVars.bAutoStab)
		return;

	if (!pLocal || !pLocal->IsAlive())
		return;

	if (!pLocal->GetActiveWeapon())
		return;

	//funny comment i found in the tf2 source code:
    /// Not scaling down the range for smaller models because midgets need all the help they can get

	if (pLocal->GetActiveWeapon()->GetClientClass()->iClassID == CTFKnife)
	{
		CGameTrace trace;
		if (DoSwingTrace(pLocal, &trace))
		{
			if (trace.m_pEnt && trace.m_pEnt->IsPlayer())
			{
				if (trace.m_pEnt->GetTeam() != pLocal->GetTeam())
				{
					if (pLocal->IsBehindAndFacingTarget(trace.m_pEnt))
					{
						pCmd->buttons |= IN_ATTACK;
					}
				}
			}
		}

	}
}

void CEnginePrediction::SetPredictionPlayer(CBaseEntity* pPlayer)
{
	//dont call this. 
	gPlayerVars.m_pPredictionPlayer = pPlayer;
}

void CEnginePrediction::StartCommand(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	pLocal->SetCurrentCommand(pCmd);
	*gPlayerVars.m_pPredictionRandomSeed = MD5_PseudoRandom(pCmd->command_number) & 255; //random_seed is 0 in clientmode creatmove.
	//pLocal->SetPredictionRandomSeed(pCmd); //in
}

void CEnginePrediction::FinishCommand(CBaseEntity* pLocal)
{
	pLocal->SetCurrentCommand(nullptr);
	*gPlayerVars.m_pPredictionRandomSeed = NULL;
	//pLocal->SetPredictionRandomSeed(NULL);
}

void CEnginePrediction::RunPreThink(CBaseEntity* pLocal)
{
	if (!pLocal->PhysicsRunThink())
		return;

	pLocal->PreThink();
}

void CEnginePrediction::RunThink(CBaseEntity* pLocal)
{
	int thinktick = pLocal->GetNextThinkTick();

	if (thinktick <= 0 || thinktick > pLocal->GetTickBase())
		return;

	pLocal->SetNextThink(TICK_NEVER_THINK);
	pLocal->Think();
}

void CEnginePrediction::RunPostThink(CBaseEntity* pLocal)
{
	pLocal->PostThink();
}

void CEnginePrediction::Start(CBaseEntity* pLocal, CUserCmd* pCmd)
{
	if (!pLocal || !pCmd)
		return;

	//i think if we disable the games prediction it fixes the double footsteps so that way maybe we dont have to
	//set prediction->InPrediction? idk i didnt notice double footsteps but then again i do play on like volume 0.0000001
	gPlayerVars.m_bSelfPredicting = true; 

	if (!gPlayerVars.m_pPredictionPlayer)
		return;

	static CMoveData moveData;
	memset(&moveData, 0, sizeof(CMoveData));

	StartCommand(pLocal, pCmd);

	m_flOldCurtime      = gInts.Globals->curtime;
	m_flOldFrameTime    = gInts.Globals->frametime;

	const float flPlayerCurtime   = pLocal->GetTickBase() * TICK_INTERVAL;
	const float flPlayerFrameTime = gInts.Engine->IsPaused() ? 0 : TICK_INTERVAL;

	gInts.Globals->curtime   = flPlayerCurtime;
	gInts.Globals->frametime = flPlayerFrameTime;

	gInts.GameMovement->StartTrackPredictionErrors(pLocal);

	if (pCmd->weaponselect != 0)
	{
		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)gInts.EntList->GetClientEntity(pCmd->weaponselect);

		if (pWeapon)
			pLocal->SelectItem(pWeapon->GetName(), pCmd->weaponsubtype); 
	}

	pLocal->UpdateButtonState(pCmd->buttons);
	pLocal->SetLocalViewAngles(pCmd->viewangles);
	
	RunPreThink(pLocal);
	RunThink(pLocal);

	gInts.Prediction->SetupMove(pLocal, pCmd, gInts.MoveHelper, &moveData);
	gInts.GameMovement->ProcessMovement(pLocal, &moveData);
	gInts.Prediction->FinishMove(pLocal, pCmd, &moveData);

	RunPostThink(pLocal);
}

void CEnginePrediction::End(CBaseEntity* pLocal)
{
	gInts.GameMovement->FinishTrackPredictionErrors(pLocal);
	FinishCommand(pLocal);

	if (gInts.Globals->frametime > 0)
		pLocal->GetTickBase()++;

	gInts.Globals->curtime   = m_flOldCurtime;
	gInts.Globals->frametime = m_flOldFrameTime;

	gPlayerVars.m_bSelfPredicting = false;
}
