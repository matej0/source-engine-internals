#include "SDK.h"
#include "CDrawManager.h"
#include "Math.h"

CBaseCombatWeapon* CBaseEntity::GetActiveWeapon()
{
	static int m_hActiveWeapon = gNetvars->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	const auto weaponData = GetValue<DWORD>(m_hActiveWeapon);
	return (CBaseCombatWeapon *)gInts.EntList->GetClientEntityFromHandle(weaponData);
}

Vector CBaseEntity::GetCollideableMins()
{
	static int m_vecMins = gNetvars->GetOffset("DT_BaseEntity", "m_Collision", "m_vecMins");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecMins);
}

Vector CBaseEntity::GetCollideableMaxs()
{
	static int m_vecMaxs = gNetvars->GetOffset("DT_BaseEntity", "m_Collision", "m_vecMaxs");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecMaxs);
}

bool CBaseEntity::ComputeHitboxSurroundingBox(Vector* mins, Vector* maxs)
{
	using ComputeHitboxSurroundingBox_t = char(__thiscall*)(PVOID, Vector*, Vector*);
	static auto dwJump = gSignatures.GetClientSignature("E9 ? ? ? ? 32 C0 5D");
	static ComputeHitboxSurroundingBox_t ComputeSurroundingBox = (ComputeHitboxSurroundingBox_t)(dwJump + 5 + *(uintptr_t*)(dwJump + 1));
	return ComputeSurroundingBox(this, mins, maxs);
}

void CBaseEntity::SetAbsOrigin(Vector& Angle)
{
	//"sprites/blueglow1.vmt"
	//55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08 F3
	using Fn = void(__thiscall*)(void*, const Vector &Angle);
	static Fn AbsOrigin = (Fn)gSignatures.GetClientSignature("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ? ? ? ? 8B 7D");
	AbsOrigin(this, Angle);
}

bool CBaseEntity::IsScoped()
{
	static int m_nPlayerCond = gNetvars->GetOffset("DT_CSPlayer", "m_bIsScoped");
	return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + m_nPlayerCond);
}

void CBaseEntity::EstimateAbsVelocity(Vector &vel)
{
	//"NPC_CombineS"
	//GetAbsVelocity: 55 8B EC 83 EC 3C 56 8B F1 F7 
	using Fn = void(__thiscall*)(PVOID, Vector &vel);
	static Fn EstAbsVel = (Fn)gSignatures.GetClientSignature("55 8B EC 83 EC 0C 56 8B F1 E8 ?? ?? ?? ?? 3B");
	EstAbsVel(this, vel);
}

Vector CBaseEntity::GetViewOffset()
{
	static int m_vecViewOffset = gNetvars->GetOffset("DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecViewOffset);
}

Vector CBaseEntity::GetEyePosition()
{
	return this->GetOrigin() + this->GetViewOffset();
}

Vector CBaseEntity::GetBonePosition(int iHitbox)
{
	matrix3x4 boneMatrix[128];
	if (this->SetupBones(boneMatrix, 128, 256, static_cast<float>(GetTickCount64())))
	{
		return Vector(boneMatrix[iHitbox][0][3], boneMatrix[iHitbox][1][3], boneMatrix[iHitbox][2][3]);
	}
	return Vector(0, 0, 0);
}

Vector CBaseEntity::GetHitboxPosition(int iHitbox, matrix3x4* BoneMatrix)
{	
	studiohdr_t* hdr = gInts.ModelInfo->GetStudiomodel(this->GetModel());
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);
	mstudiobbox_t* hitbox = set->GetHitbox(iHitbox);

	if (!this->SetupBones(BoneMatrix, 128, 0x100, 0))
		return Vector(0, 0, 0);

	if (hitbox)
	{
		Vector vMin, vMax, vCenter, sCenter;
		gMath.VectorTransform(hitbox->bbmin, BoneMatrix[hitbox->bone], vMin);
		gMath.VectorTransform(hitbox->bbmax, BoneMatrix[hitbox->bone], vMax);
		vCenter = (vMin + vMax) * 0.5;

		return vCenter;
	}

	return Vector(0, 0, 0);
}


bool &CBaseEntity::IsSpotted()
{
	static int m_bSpotted = gNetvars->GetOffset("DT_BaseEntity", "m_bSpotted");
	return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + m_bSpotted);
}

Vector CBaseEntity::EyeAngles()
{
	static int m_angEyeAngles = gNetvars->GetOffset("DT_CSPlayer", "m_angEyeAngles");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_angEyeAngles);
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

int& CBaseEntity::GetFlags()
{
	static int m_fFlags = gNetvars->GetOffset("DT_BasePlayer", "m_fFlags");
	return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_fFlags);
}

Vector CBaseEntity::GetOrigin()
{
	static int m_vecOrigin = gNetvars->GetOffset("DT_BaseEntity", "m_vecOrigin");
	return *reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(this) + m_vecOrigin);
}

int &CBaseEntity::GetTickBase()
{
	static int m_nTickBase = gNetvars->GetOffset("DT_BasePlayer", "localdata", "m_nTickBase");
	return *reinterpret_cast<int*>(uintptr_t(this) + m_nTickBase);
}

float CBaseEntity::GetSimulationTime()
{
	static int m_flSimulationTime = gNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime");
	return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flSimulationTime);
}

Vector CBaseEntity::GetAimPunchAngle()
{
	return *(Vector*)((DWORD)this + 0x302C);
}

int* CBaseEntity::GetWeapons()
{
	static int m_hMyWeapons = NetVarManager->GetOffset("DT_BaseCombatCharacter", "m_hMyWeapons") / 2;
	return (int*)((uintptr_t)this + m_hMyWeapons);
}

int CBaseEntity::GetSequenceActivity(int sequence)
{
	studiohdr_t* pHDR = gInts.ModelInfo->GetStudiomodel(this->GetModel());
	if (!pHDR)
		return -1;

	static auto fnGetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(gSignatures.GetClientSignature("55 8B EC 53 8B 5D 08 56 8B F1 83"));
	return fnGetSequenceActivity(this, pHDR, sequence);
}

void CBaseEntity::InvalidatePhysicsRecursive(InvalidatePhysicsBits_t nFlag)
{
	using InvalidatePhysicsRecursive_t = void(__thiscall *)(decltype(this), InvalidatePhysicsBits_t);
	static InvalidatePhysicsRecursive_t InvalidatePhysicsRecursiveFn = (InvalidatePhysicsRecursive_t)gSignatures.GetClientSignature("55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3");
	InvalidatePhysicsRecursiveFn(this, nFlag);
}

int CBaseEntity::GetShotsFired()
{
	static int m_iShotsFired = NetVarManager->GetOffset("DT_CSPlayer", "m_iShotsFired");
	return *(int*)((uintptr_t)this + m_iShotsFired);
}