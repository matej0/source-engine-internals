#include "Aimbot.h"
CAimbot gAimbot;
ISort gTargetSort;

std::vector<IPlayerData> m_Targets = { }; //store all targets in a list.

bool CAimbot::IsVisible(CBaseEntity *pLocal, CBaseEntity *pEntity, Vector &src, Vector &dst)
{
	CTraceFilter filter;
	filter.pSkip1 = pLocal;

	Ray_t ray;
	ray.Init(src, dst);

	trace_t trace;
	gInts.EngineTrace->TraceRay(ray, 0x46004003, &filter, &trace);


	return (trace.m_pEnt == pEntity || trace.flFraction == 1.f);
}

bool CAimbot::GetHitboxes(CBaseEntity* pLocal, CBaseEntity* pEntity, std::vector<int> &hitboxes)
{
	hitboxes.clear();

	if (gVars.iHitboxes == 0) //min = head and some body.
	{
		hitboxes.push_back(HITBOX_HEAD);
		hitboxes.push_back(HITBOX_CHEST);
		hitboxes.push_back(HITBOX_PELVIS);
	}
	else if (gVars.iHitboxes == 1) //mid = head and all body.
	{
		hitboxes.push_back(HITBOX_HEAD);
		hitboxes.push_back(HITBOX_BODY);
		hitboxes.push_back(HITBOX_UPPER_CHEST);
		hitboxes.push_back(HITBOX_CHEST);
		hitboxes.push_back(HITBOX_THORAX);
		hitboxes.push_back(HITBOX_PELVIS);
	}
	else if (gVars.iHitboxes == 2) //max = all hitboxes. prolly quite expensive XD.
	{
		for (int i = 0; i < HITBOX_MAX; i++)
		{
			if (i == HITBOX_L_FOOT || i == HITBOX_R_FOOT)
				continue; //these are cubes. we dont do no cubes.

			hitboxes.push_back(i);
		}
	}

	if (hitboxes.empty())
		return false; //idk shouldnt happen.

	return true;
}

bool CAimbot::GetHitboxPoints(CBaseEntity* pEntity, matrix3x4 *bones, int index, std::vector< Vector > &points) 
{
	points.clear();

	const DWORD *pModel = pEntity->GetModel();
	if (!pModel)
		return false;

	studiohdr_t *pHDR = gInts.ModelInfo->GetStudiomodel(pModel);
	if (!pHDR)
		return false;

	mstudiohitboxset_t *pHitboxSet = pHDR->GetHitboxSet(0);
	if (!pHitboxSet)
		return false;

	mstudiobbox_t *bbox = pHitboxSet->GetHitbox(index);
	if (!bbox)
		return false;

	float scale = gVars.nMultiPointScale / 100.f;
	float bscale = gVars.nMultiPointScale / 100.f;

	if (bbox->m_flRadius > 0 && bbox->m_flRadius != -1.f)
	{
		float r = bbox->m_flRadius * scale;
		float br = bbox->m_flRadius * bscale;
		Vector center = (bbox->bbmin + bbox->bbmax) / 2.f;

		if (index == HITBOX_HEAD) 
		{
			points.push_back(center);

			if (gVars.bMultiPoint) 
			{
				constexpr float rotation = 0.70710678f;
				points.push_back({ bbox->bbmax.x + (rotation * r), bbox->bbmax.y + (-rotation * r), bbox->bbmax.z });
				points.push_back({ bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + r });
				points.push_back({ bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - r });
				points.push_back({ bbox->bbmax.x, bbox->bbmax.y - r, bbox->bbmax.z });
			}
		}
		else if (index == HITBOX_PELVIS) 
		{
			points.push_back(center);

			if (gVars.bMultiPoint)
				points.push_back({ center.x, bbox->bbmax.y - br, center.z });
		}
		else if (index == HITBOX_PELVIS || index == HITBOX_UPPER_CHEST) 
		{
			points.push_back({ center.x, bbox->bbmax.y - r, center.z });
		}
		else if (index == HITBOX_THORAX || index == HITBOX_CHEST) 
		{

			points.push_back(center);

			if (gVars.bMultiPoint)
				points.push_back({ center.x, bbox->bbmax.y - r, center.z });
		}
		else if (index == HITBOX_R_CALF || index == HITBOX_L_CALF) 
		{
			points.push_back(center);

			if (gVars.bMultiPoint)
				points.push_back({ bbox->bbmax.x - (bbox->m_flRadius / 2.f), bbox->bbmax.y, bbox->bbmax.z });
		}
		else if (index == HITBOX_R_THIGH || index == HITBOX_L_THIGH) 
		{
			points.push_back(center);
		}
		else if (index == HITBOX_R_UPPER_ARM || index == HITBOX_L_UPPER_ARM) 
		{
			points.push_back({ bbox->bbmax.x + bbox->m_flRadius, center.y, center.z });
		}

		if (points.empty())
			return false;

		for (auto &p : points)
			gMath.VectorTransform(p, bones[bbox->bone], p);
	}

	return true;
}

Vector CAimbot::GetAimPoint(CBaseEntity* pLocal, CBaseEntity* pEntity, CUserCmd* pCommand)
{
	Vector vecHitbox = Vector();
	std::vector<Vector> rgAimPoints; 
	std::vector<int>    rgHitboxes;
	if (!gAimbot.GetHitboxes(pLocal, pEntity, rgHitboxes))
		return Vector(0, 0, 0);

	for (auto &hitbox : rgHitboxes)
	{
		if (!gAimbot.GetHitboxPoints(pEntity, gAimbot.m_Matrix[pEntity->GetIndex()], hitbox, rgAimPoints))
			continue;

		gVars.DEBUG.nAimbotPoints = rgAimPoints.size();

		for (auto& point : rgAimPoints)
		{
			if (gAimbot.IsVisible(pLocal, pEntity, pLocal->GetEyePosition(), point))
				return point;
		}
	}

	return Vector(0, 0, 0);
}

void CAimbot::GetBestTargets(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	Vector vLocal = pLocal->GetEyePosition();
	float flFovLimit = gVars.bFOV;

	if (gVars.bRageMode)
		flFovLimit = 360.f;

	for (int i = 1; i <= gInts.Globals->maxClients; i++)
	{
		if (i == me)
			continue;

		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->GetTeam() == pLocal->GetTeam())
			continue;

		float flFOV = gMath.GetFOV(pCommand->viewangles, vLocal, pEntity->GetEyePosition());

		if (flFOV > flFovLimit)
			continue;

		//add to the list.
		IPlayerData pData;
		pData.m_Index = i;
		pData.m_pEntity = pEntity;
		pData.m_Fov = flFOV;
		m_Targets.emplace_back(pData);
	}
}

void CAimbot::SetToUninterpolated(CBaseEntity* pEntity, CBaseEntity* pLocal)
{
	if (!pEntity || pEntity == pLocal || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->GetTeam() == pLocal->GetTeam())
		return;

	Vector vBackup = pEntity->GetAbsOrigin();

	pEntity->SetAbsOrigin(pEntity->GetOrigin()); // set to uninterpolated data.

	if (!pEntity->SetupBones(gAimbot.m_Matrix[pEntity->GetIndex()], 128, 0x100, 0))
		return;

	//gAim.StoreRecord(pLocal, i);

	pEntity->SetAbsOrigin(vBackup); //restore to interpolated data.
}

bool IsAccurate(CBaseEntity* pLocal, CBaseEntity* pEntity, float flHitchance, Vector& vAngle)
{
	CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon();

	if (!pWeapon)
		return false;

	CSWeaponData* pWeaponData = pWeapon->GetCSWpnData();

	if (!pWeaponData)
		return false;

	auto bIsCorrectHitgroup = [](int index) -> bool
	{
		return ((index >= HITGROUP_HEAD && index <= HITGROUP_RIGHTLEG) || index == HITGROUP_GEAR);
	}; 

	Vector vForward, vStart, vEnd;

	int hits = 0;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	vForward = vAngle;

	filter.pSkip1 = pLocal;
	vStart = pLocal->GetEyePosition();


	float flSpread = pWeapon->GetSpread(), flInaccuracy = pWeapon->GetInaccuracy(), flRange = pWeaponData->flRange;
	float flCalcedSpread = flInaccuracy + flSpread;

	for (int i = 0; i < 100; i++)
	{

		Vector vRandom = { gMath.RandomFloat(-(flCalcedSpread / 2), flCalcedSpread / 2),
			gMath.RandomFloat(-(flCalcedSpread / 2), flCalcedSpread / 2),
			gMath.RandomFloat(-(flCalcedSpread / 2), flCalcedSpread / 2) };

		auto randomized_forward = vForward + vRandom;
		ray.Init(vStart, vStart + randomized_forward * flRange);
		gInts.EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

		if (tr.m_pEnt == pEntity) //&& bIsCorrectHitgroup(tr.hitGroup))
			hits++;
	}

	if (((static_cast<float>(hits) / 100) * 100.f) >= flHitchance)
		return true;

	return false;
}

bool CanFire(CBaseEntity* pLocal)
{
	float flServerTime = pLocal->GetTickBase() * gInts.Globals->interval_per_tick;
	return (pLocal->GetNextAttack() <= flServerTime);
}

void CAimbot::Think(CUserCmd* pCommand, CBaseEntity* pLocal)
{
	if (!gVars.bAimbot)
		return;

	if (!(pCommand->buttons & IN_ATTACK) && !gVars.bRageMode)
		return;

	if (!pLocal->GetActiveWeapon())
		return;

	if (!CanFire(pLocal))
		return;

	m_Targets.clear();
	gAimbot.GetBestTargets(pLocal, pCommand);

	if (m_Targets.empty())
		return;

	CBaseEntity* pBestTarget = nullptr;
	Vector       vecBestHitbox = Vector();

	std::sort(m_Targets.begin(), m_Targets.end(), gTargetSort); //sort the fuckers.

	for (auto& target : m_Targets)
	{
		gAimbot.SetToUninterpolated(target.m_pEntity, pLocal);

		vecBestHitbox = gAimbot.GetAimPoint(pLocal, target.m_pEntity, pCommand);

		if (vecBestHitbox == Vector(0, 0, 0))
			continue;

		pBestTarget = target.m_pEntity;
		break;
	}

	if (!pBestTarget)
		return;

	Vector Start = pLocal->GetEyePosition();

	Vector Target = vecBestHitbox;

	Vector Angle;

	VectorAngles((Target - Start), Angle);

	if (gVars.bRCS)
		Angle -= pLocal->GetAimPunchAngle() * 2.f;

	ClampAngle(Angle);

	if (!gVars.bRageMode)
	{
		auto GetSmoothingAmount = []() -> float
		{
			float flSmoothing = 0.f;

			if (gVars.bSmooth == 1)
				flSmoothing = 7.f;
			else if (gVars.bSmooth == 2)
				flSmoothing = 21.f;
			else
				flSmoothing = 0.f;

			return flSmoothing;
		};

		if (gVars.bSmooth > 0)
		{
			Vector qCurrentView, qDelta, qAimDirecction = Angle;
			gInts.Engine->GetViewAngles(qCurrentView);
			qDelta = qAimDirecction - qCurrentView;

			//eh
			if (qDelta[0] > 180)  qDelta[0] -= 360;
			if (qDelta[1] > 180)  qDelta[1] -= 360;
			if (qDelta[0] < -180) qDelta[0] += 360;
			if (qDelta[1] < -180) qDelta[1] += 360;

			qAimDirecction = qCurrentView + qDelta / GetSmoothingAmount();
			pCommand->viewangles = qAimDirecction;
			gInts.Engine->SetViewAngles(pCommand->viewangles);
		}
		else
		{
			pCommand->viewangles = Angle;

			if (!gVars.bSilent)
				gInts.Engine->SetViewAngles(pCommand->viewangles);
		}
	}
	else
	{
		pCommand->viewangles = Angle;

		if (!gVars.bSilent)
			gInts.Engine->SetViewAngles(pCommand->viewangles);
	}
	

	if (gVars.bAutoShoot)
	{

	}
	

	//pCommand->tick_count = TIME_TO_TICKS(pBestTarget->GetSimulationTime() + GetLerpTime());
}

