#include "SDK.h"

class CNetChannel;
class CPlayerVariables
{
public:
	DWORD dwPureReturnAddress;
	DWORD dwFogReturnAddress;

	bool* bSendPacket;
	bool bIsHoldingKnife;
	
	float bSkinChanger;
	float bESP;
	float bShowInfo;
	float bHealthBar;
	float bBox;
	float bRadar;
	float bAimbot;
	float bSmooth;
	float bSilent;
	float bAutoShoot;
	float bMultiPoint;
	float nMultiPointScale = 10.f;
	float bAutowall;
	float nAutoWallDmg;
	float bFOV = 0.f;
	float bRCS;
	float bRageMode;
	float bStandaloneRCS;
	float iHitboxes;
	float iBone = 1;
	float bTriggerbot;
	float bBunnyhop;
	float bXhair;
	float bChams;
	float bLagChams;
	float bXQZChams;
	float bSkeleton;
	float bBacktracking;
	float nBacktrackTicks = 12.f;
	float bWeaponChams;

	float  iFov = 90;
	float  iVMFov = 68;

	float iMenu_Pos_X = 100;
	float iMenu_Pos_Y = 500;
public:
	struct
	{
		int nSequence;
		bool bPassedSanityChecks;
		bool bIsViewModelPointerValid;
		bool bOriginalWasCalled;
		bool bIsCorrectKnife;
		float flTimeUntilNextShot;
		int nTicksUntilNextShot;
		float flNextAttack;
		const char* szModelName = "";
		DWORD* pOwnerAddress;
		int nAimbotPoints;
	} DEBUG;
};


class CConvars
{
public:
	ConVar* cl_updaterate;
	ConVar* cl_cmdrate;
	ConVar* cl_interp;
	ConVar* cl_interp_ratio;
	ConVar* glow_outline_effect_enable;
	ConVar* sv_minupdaterate;
	ConVar* sv_maxupdaterate;
	ConVar* sv_client_min_interp_ratio;
	ConVar* sv_client_max_interp_ratio;
	ConVar* sv_airaccelerate;
	ConVar* weapon_recoil_scale;
	ConVar* cl_recoil_crosshair;
};

class COffsets
{
public:
	int iCreateMoveOffset = 21, iPaintTraverseOffset = 41;
};