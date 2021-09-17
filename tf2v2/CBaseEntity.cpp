#include "SDK.h"
#include "CDrawManager.h"
#include "Aimbot.h"



DWORD* CBaseCombatWeapon::GetWpnData()
{
	//0F B7 81 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 C3
	typedef DWORD*(__thiscall* GetWpnDataFn)(void*);
	static DWORD dwInstructionPtr = gSignatures.GetClientSignature("E8 ? ? ? ? 83 B8 ? ? ? ? ? 74 24");
	static DWORD dwMask = dwInstructionPtr + 1;
	static DWORD dwFunction = ((*(PDWORD)(dwMask)) + dwMask + 4);

	static GetWpnDataFn WpnData = (GetWpnDataFn)(dwFunction);
	return WpnData(this);
}

CBaseCombatWeapon* CBaseEntity::GetActiveWeapon()
{
	static int m_hActiveWeapon = gNetvars->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	const auto weaponData = GetValue<DWORD>(m_hActiveWeapon);
	return (CBaseCombatWeapon *)gInts.EntList->GetClientEntityFromHandle(weaponData);
}

CBaseCombatWeapon* CBaseEntity::GetWeaponFromSlot(int slot)
{
	static int m_hMyWeapons = gNetvars->GetOffset("DT_BaseCombatCharacter", "m_hMyWeapons") + slot * 4;
	const auto weaponData = GetValue<DWORD>(m_hMyWeapons);
	return (CBaseCombatWeapon *)gInts.EntList->GetClientEntityFromHandle(weaponData);
}

Vector CBaseEntity::GetCollideableMins()
{
	static int m_vecMins = gNetvars->GetOffset("DT_BaseEntity", "m_vecMins");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecMins);
}

Vector CBaseEntity::GetCollideableMaxs()
{
	static int m_vecMaxs = gNetvars->GetOffset("DT_BaseEntity", "m_vecMaxs");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecMaxs);
}

void CBaseEntity::UpdateGlowEffect()
{
	typedef void(__thiscall* UpdateGlowEffectFn)(void*);
	getvfunc<UpdateGlowEffectFn>(this, 226)(this);
}

void CBaseEntity::DestroyGlowEffect()
{
	typedef void(__thiscall* DestroyGlowEffectFn)(void*);
	getvfunc<DestroyGlowEffectFn>(this, 227)(this);
}

bool &CBaseEntity::GlowEnabled()
{
	static int m_bGlowEnabled = gNetvars->GetOffset("DT_TFPlayer", "m_bGlowEnabled");
	return *reinterpret_cast<bool*>(uintptr_t(this) + m_bGlowEnabled);
}

Vector CBaseEntity::GetEyeAngles()
{
	static int m_angEyeAngles = gNetvars->GetOffset("DT_TFPlayer", "m_angEyeAngles[0]");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_angEyeAngles);
}

bool CBaseEntity::IsBehindAndFacingTarget(CBaseEntity *pTarget)
{
	Vector vTargetWorldSpaceCenter, vOwnerWorldSpaceCenter;

	pTarget->GetWorldSpaceCenter(vTargetWorldSpaceCenter);
	this->GetWorldSpaceCenter(vOwnerWorldSpaceCenter);

	Vector vecToTarget;
	vecToTarget = vTargetWorldSpaceCenter - vOwnerWorldSpaceCenter;
	vecToTarget.z = 0.0f;
	vecToTarget.NormalizeInPlace();

	// Get owner forward view vector
	Vector vecOwnerForward;
	AngleVectors(this->GetEyePosition(), &vecOwnerForward, NULL, NULL);
	vecOwnerForward.z = 0.0f;
	vecOwnerForward.NormalizeInPlace();

	// Get target forward view vector
	Vector vecTargetForward;
	AngleVectors(pTarget->GetEyePosition(), &vecTargetForward, NULL, NULL);
	vecTargetForward.z = 0.0f;
	vecTargetForward.NormalizeInPlace();

	// Make sure owner is behind, facing and aiming at target's back
	float flPosVsTargetViewDot = DotProduct(vecToTarget, vecTargetForward);	// Behind?
	float flPosVsOwnerViewDot = DotProduct(vecToTarget, vecOwnerForward);		// Facing?
	float flViewAnglesDot = DotProduct(vecTargetForward, vecOwnerForward);	// Facestab?

	return (flPosVsTargetViewDot > 0.f && flPosVsOwnerViewDot > 0.5 && flViewAnglesDot > -0.3f);
}

CBaseEntity* CBaseEntity::GetHealingTarget()
{
	CBaseEntity* pHealingTarget = nullptr;

	if (this->GetClassNum() == TF2_Medic && this->GetActiveWeapon()->GetSlot() == TF_WEAPONSLOT_SECONDARY)
	{
		static auto m_hHealingTarget = gNetvars->GetOffset("DT_WeaponMedigun", "m_hHealingTarget");
		int hTarget = *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_hHealingTarget);
		pHealingTarget = gInts.EntList->GetClientEntityFromHandle(hTarget);
	}

	return pHealingTarget;
}

bool CBaseEntity::IsHoldingProjectileWeapon()
{
	if (this->GetClassNum() == TF2_Soldier && this->GetActiveWeapon()->GetSlot() == TF_WEAPONSLOT_PRIMARY)
		return true;

	if (this->GetClassNum() == TF2_Demoman && this->GetActiveWeapon()->GetSlot() == TF_WEAPONSLOT_PRIMARY
		&& this->GetActiveWeapon()->GetItemDefinitionIndex() != demomanweapons::WPN_WeeBooties
		&& this->GetActiveWeapon()->GetItemDefinitionIndex() != demomanweapons::WPN_BootLegger
		&& this->GetActiveWeapon()->GetItemDefinitionIndex() != demomanweapons::WPN_BaseJumper)
		return true;

	switch (this->GetActiveWeapon()->GetItemDefinitionIndex())
	{
	case sniperweapons::WPN_Huntsman: case sniperweapons::WPN_FestiveHuntsman: case sniperweapons::WPN_CompoundBow:
		return true;
	case pyroweapons::WPN_Flaregun: case pyroweapons::WPN_FestiveFlaregun: case pyroweapons::WPN_Detonator: case pyroweapons::WPN_ScorchShot: case pyroweapons::WPN_ManMelter:
		return true;
	case medicweapons::WPN_Crossbow: case medicweapons::WPN_FestiveCrossbow: case medicweapons::WPN_SyringeGun: case medicweapons::WPN_Blutsauger:
		return true;
	case engineerweapons::WPN_RescueRanger:
		return true;
	}

	return false;
}



//works fine with most weapons except minigun and maybe smg ( havent tested ).
bool CBaseEntity::CanFireWithCurrentWeapon()
{
	//https://www.unknowncheats.me/forum/team-fortress-2-a/273821-canshoot-function.html
	if (this->GetActiveWeapon() == nullptr)
		return false;

	static float flLastFire = 0, flNextAttack = 0;
	static CBaseCombatWeapon* pOldWeapon;

	if (flLastFire != this->GetActiveWeapon()->GetLastFireTime() || this->GetActiveWeapon() != pOldWeapon)
	{
		flLastFire = this->GetActiveWeapon()->GetLastFireTime();
		flNextAttack = this->GetActiveWeapon()->GetNextPrimaryAttack();
	}

	if (this->GetActiveWeapon()->GetClip1() == 0)
		return false;

	pOldWeapon = this->GetActiveWeapon();

	return (flNextAttack <= (float)(this->GetTickBase()) * gInts.Globals->interval_per_tick);
}

bool CBaseEntity::ComputeHitboxSurroundingBox(Vector* mins, Vector* maxs)
{
	//thank you IAmWolfie however, i did find the sig myself, i was just running it on buildings too.
	//turns out it crashes if not called on a player. 
	//adding 1 byte to it should probably work too because all you really want is to skip the jmp instruction. 
	//but w/e
	//UPDATE: it wouldnt work, you need to get the relative address.
	using ComputeHitboxSurroundingBox_t = char(__thiscall*)(PVOID, Vector*, Vector*);
	static auto jmp = gSignatures.GetClientSignature("E9 ? ? ? ? FF 75 0C 8B CE");
	static ComputeHitboxSurroundingBox_t ComputeSurroundingBox = (ComputeHitboxSurroundingBox_t)(jmp + 5 + *(uintptr_t*)(jmp + 1));
	return ComputeSurroundingBox(this, mins, maxs);
}

void CBaseEntity::AddFlag(int flag)
{
	using Fn = void(__thiscall*)(void*, int flag);
	static Fn Add = (Fn)gPlayerVars.dwAddFlag;
	Add(this, flag);
}

void CBaseEntity::SetAbsOrigin(Vector& Angle)
{
	//"sprites/blueglow1.vmt"
	//55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08 F3
	using Fn = void(__thiscall*)(void*, const Vector &Angle);
	static Fn AbsOrigin = (Fn)gSignatures.GetClientSignature("55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08 F3");
	AbsOrigin(this, Angle);
}

void CBaseEntity::SetAbsAngles(Vector& Angle)
{

    //55 8B EC 83 EC 60 56 57 8B F1
	using Fn = void(__thiscall*)(void*, const Vector &Angle);
	static Fn AbsAngles = (Fn)gSignatures.GetClientSignature("55 8B EC 83 EC 60 56 57 8B F1");
	AbsAngles(this, Angle);
}


int CBaseEntity::GetClassNum()
{
	static int m_iClass = gNetvars->GetOffset("DT_TFPlayer", "m_iClass");
	return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iClass);
}

int& CBaseEntity::GetCond()
{
	static int m_nPlayerCond = gNetvars->GetOffset("DT_TFPlayer", "m_nPlayerCond");
	return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_nPlayerCond);
}

CBaseEntity* CBaseEntity::GetObserverTarget()
{
	return gInts.EntList->GetClientEntityFromHandle(hObserverTarget());
}

//55 8B EC 83 EC 0C 56 57 8B 7D 08 8B F1 F3 SetAbsVelocity
void CBaseEntity::EstimateAbsVelocity(Vector &vel)
{
	//"NPC_CombineS"
	//GetAbsVelocity: 55 8B EC 83 EC 3C 56 8B F1 F7 
	using Fn = void(__thiscall*)(PVOID, Vector &vel);
	static Fn EstAbsVel = (Fn)gSignatures.GetClientSignature("55 8B EC 83 EC 0C 56 8B F1 E8 ?? ?? ?? ?? 3B");
	EstAbsVel(this, vel);
}


float CBaseEntity::GetConveyorSpeed()
{
	static int m_flConveyorSpeed = gNetvars->GetOffset("DT_FuncConveyor", "m_flConveyorSpeed");
	return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_flConveyorSpeed);
}

bool CBaseEntity::IsOnGround()
{
	return (this->GetFlags() & FL_ONGROUND);
}

CBaseEntity* CBaseEntity::GetGroundEntity()
{
	return nullptr;//gInts.EntList->GetClientEntityFromHandle(hGroundEntity());
}

Vector CBaseEntity::GetBaseVelocity()
{
	static int m_vecBaseVelocity = gNetvars->GetOffset("DT_BasePlayer", "m_vecBaseVelocity");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecBaseVelocity);
}

Vector CBaseEntity::GetViewOffset()
{
	static int m_vecViewOffset = gNetvars->GetOffset("DT_BasePlayer", "m_vecViewOffset[0]");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecViewOffset);
}

Vector CBaseEntity::GetEyePosition()
{
	return this->GetOrigin() + this->GetViewOffset();
}

int CBaseEntity::GetHealth()
{
	static int m_iHealth = gNetvars->GetOffset("DT_BasePlayer", "m_iHealth");
	return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iHealth);
}

bool CBaseEntity::IsAlive()
{
	return this->GetLifeState() == LIFE_ALIVE;
}

int CBaseEntity::GetTeam()
{
	static int m_iTeam = gNetvars->GetOffset("DT_BaseEntity", "m_iTeamNum");
	return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iTeam);
}

BYTE CBaseEntity::GetLifeState()
{
	static int m_lifeState = gNetvars->GetOffset("DT_BasePlayer", "m_lifeState");
	return *reinterpret_cast<BYTE*>(reinterpret_cast<std::uintptr_t>(this) + m_lifeState);
}

int CBaseEntity::GetHitboxSet()
{
	static int m_nHitboxSet = gNetvars->GetOffset("DT_BaseAnimating", "m_nHitboxSet");
	return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_nHitboxSet);
}

void VectorTransform(const Vector& vSome, const matrix3x4& vMatrix, Vector& vOut)
{
	for (auto i = 0; i < 3; i++)
		vOut[i] = vSome.Dot((Vector&)vMatrix[i]) + vMatrix[i][3];
}

Vector CBaseEntity::GetHitboxPositionCustom(int iHitbox, matrix3x4* BoneMatrix)
{
	studiohdr_t* hdr = gInts.ModelInfo->GetStudiomodel(this->GetModel());
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	mstudiobbox_t* hitbox = set->pHitbox(iHitbox);

	if (hitbox)
	{
		Vector vMin, vMax, vCenter, sCenter;
		VectorTransform(hitbox->bbmin, BoneMatrix[hitbox->bone], vMin);
		VectorTransform(hitbox->bbmax, BoneMatrix[hitbox->bone], vMax);
		vCenter = (vMin + vMax) * 0.5;

		return vCenter;
	}

	return Vector(0, 0, 0);
}

Vector CBaseEntity::GetHitboxPosition(int iHitbox)
{

	matrix3x4 BoneMatrix[128];
	if (!this->SetupBones(BoneMatrix, 128, 0x100, gInts.Globals->curtime))
		return Vector();

	studiohdr_t* hdr = gInts.ModelInfo->GetStudiomodel(this->GetModel());
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	mstudiobbox_t* hitbox = set->pHitbox(iHitbox);

	if (hitbox)
	{
		Vector vMin, vMax, vCenter, sCenter;
		VectorTransform(hitbox->bbmin, BoneMatrix[hitbox->bone], vMin);
		VectorTransform(hitbox->bbmax, BoneMatrix[hitbox->bone], vMax);
		vCenter = (vMin + vMax) * 0.5;

		return vCenter;
	}

	return Vector(0, 0, 0);
}

Vector CBaseEntity::PredictedPoint(CBaseEntity* pLocal, Vector vHitbox, Vector vVelocity)
{
	float distance = pLocal->GetEyePosition().DistTo(vHitbox);

	float missileSpeed = pLocal->GetActiveWeapon()->GetProjectileSpeed();

	float timeToTravel = distance / missileSpeed;

	Vector leadTargetSpot;
	leadTargetSpot.x = vHitbox.x + timeToTravel * vVelocity.x;
	leadTargetSpot.y = vHitbox.y + timeToTravel * vVelocity.y;

	if (!(this->GetFlags() & FL_ONGROUND))
		leadTargetSpot.z = vHitbox.z + ((0.5 * -800) * powf(timeToTravel, 2)) + (vVelocity.z * timeToTravel);
	else
		leadTargetSpot.z = vHitbox.z + timeToTravel * vVelocity.z;

	float predictedDistance = pLocal->GetEyePosition().DistTo(leadTargetSpot);

	float elevationAngle = predictedDistance * 0.00060f;//0.00075;

	if (elevationAngle > 45.0f)
		elevationAngle = 45.0f;

	//some math stuff that i cannot comprehend lol.
	float s, c;
	SinCos((elevationAngle * PI / 180.0f), &s, &c);

	if (c)
	{
		float elevation = predictedDistance * (s / c);
		return leadTargetSpot + Vector(0, 0, elevation);
	}

	return leadTargetSpot;
}


BYTE& CBaseEntity::GetFlags()
{
	static int m_fFlags = gNetvars->GetOffset("DT_BasePlayer", "m_fFlags");
	return *reinterpret_cast<BYTE*>(reinterpret_cast<std::uintptr_t>(this) + m_fFlags);
}

Vector CBaseEntity::GetAbsVelocity()
{
	static int m_vecAbsVelocity = gNetvars->GetOffset("DT_BaseEntity", "m_vecAbsVelocity");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecAbsVelocity);
}

Vector CBaseEntity::GetOrigin()
{
	static int m_vecOrigin = gNetvars->GetOffset("DT_BaseEntity", "m_vecOrigin");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecOrigin);
}

Vector CBaseEntity::GetNetworkedAngles()
{
	static int m_angRotation = gNetvars->GetOffset("DT_BaseEntity", "m_angRotation");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_angRotation);
}


bool CBaseEntity::IsPlayer()
{
	return (this->GetClientClass()->iClassID == CTFPlayer);
}

bool CBaseEntity::IsBuilding()
{
	return (this->GetClientClass()->iClassID == CObjectSentrygun ||
		    this->GetClientClass()->iClassID == CObjectDispenser ||
		    this->GetClientClass()->iClassID == CObjectTeleporter);
}

int &CBaseEntity::GetTickBase()
{
	static int m_nTickBase = gNetvars->GetOffset("DT_BasePlayer", "m_nTickBase");
	return *reinterpret_cast<int*>(uintptr_t(this) + m_nTickBase);
}

//getsequence
//124
//updateclientsideabunatuinb
//190 - 194
//56 8B F1 80 BE ? ? ? ? ? 74 27
//56 57 8B F9 33 F6 6A
void CBaseEntity::UpdateClientSideAnimations()
{
	using Fn = void(__thiscall*)(void*);
	static Fn UpdateAnims = (Fn)gSignatures.GetClientSignature("55 8B EC 83 EC 0C 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 74 10 8B 17 8B CF 8B 92 ? ? ? ? FF D2");
	UpdateAnims(this);
}

float CBaseEntity::GetSimulationTime()
{
	static int m_flSimulationTime = gNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime");
	return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flSimulationTime);
}

float CBaseEntity::GetOldSimulationTime()
{
	/*
	in C_BasePlayer::PostDataUpdate
	float flTimeDelta = m_flSimulationTime - m_flOldSimulationTime;
	if ( flTimeDelta > 0...)
    =
	if ( (float)(*(float *)(a1 + 0x5C) - *(float *)(a1 + 0x60)) > 0.0...)

	0x60 - 0x5C = 0x4
	*/

	static int m_flOldSimulationTime = gNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime") + 0x4;
	return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flOldSimulationTime);
}

bool &CBaseEntity::ClientSideAnimation()
{
	static int m_bClientSideAnimation = gNetvars->GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
	return *reinterpret_cast<bool*>(uintptr_t(this) + m_bClientSideAnimation);
}

bool CBaseEntity::Ubered()
{
	return (this->GetCond() & TFCond_Ubercharged || this->GetCond() & TFCond_UberchargeFading);
}

int CBaseEntity::GetOwner()
{
	static int m_hOwnerEntity = gNetvars->GetOffset("DT_BaseEntity", "m_hOwnerEntity");
	return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_hOwnerEntity);
}