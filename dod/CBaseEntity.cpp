#include "SDK.h"
#include "CDrawManager.h"

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
	static auto jmp = gSignatures.GetClientSignature("E9 ? ? ? ? FF 75 0C 8B CE");
	static ComputeHitboxSurroundingBox_t ComputeSurroundingBox = (ComputeHitboxSurroundingBox_t)(jmp + 5 + *(uintptr_t*)(jmp + 1));
	return ComputeSurroundingBox(this, mins, maxs);
}


CBaseEntity* CBaseEntity::GetObserverTarget()
{
	return gInts.EntList->GetClientEntityFromHandle(hObserverTarget());
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

Vector CBaseEntity::GetHitboxPosition(int iHitbox)
{
	DWORD *pModel = this->GetModel();

	if (!pModel)
		return Vector();

	studiohdr_t *pHdr = gInts.ModelInfo->GetStudiomodel(pModel);

	if (!pHdr)
		return Vector();

	matrix3x4 BoneMatrix[128];
	if (!this->SetupBones(BoneMatrix, 128, 0x100, gInts.Globals->curtime))
		return Vector();

	mstudiohitboxset_t *pSet = pHdr->GetHitboxSet(this->GetHitboxSet());
	if (!pSet)
		return Vector();

	mstudiobbox_t *pBox = pSet->pHitbox(iHitbox);
	if (!pBox)
		return Vector();

	Vector vPos = (pBox->bbmin + pBox->bbmax) * 0.5f;
	Vector vOut = Vector();

	VectorTransform(vPos, BoneMatrix[pBox->bone], vOut);

	return vOut;
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
//getsequence
//124

//updateclientsideabunatuinb
//190 - 194
//56 8B F1 80 BE ? ? ? ? ? 74 27

void CBaseEntity::UpdateClientSideAnimation()
{
	typedef void(__thiscall* Fn)(PVOID);
	getvfunc<Fn>(this, 194)(this);
}

float CBaseEntity::GetSimulationTime()
{
	static int m_flSimulationTime = gNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime");
	return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flSimulationTime);
}

void CBaseEntity::Aids()
{
	//56 57 8B F9 33 F6 6A
	//56 8B F1 80 BE ? ? ? ? ? 74 27
	static const auto upd = reinterpret_cast<void(__thiscall*)(CBaseEntity*)>(gSignatures.GetClientSignature("56 57 8B F9 33 F6 6A"));
	upd(this);
}

bool &CBaseEntity::ClientSideAnimation()
{
	static int m_bClientSideAnimation = gNetvars->GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
	return *reinterpret_cast<bool*>(uintptr_t(this) + m_bClientSideAnimation);
}

