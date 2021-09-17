#include "SDK.h"

class CNetChannel;

enum AimbotIgnores_t
{
	CLOAKED,
	UBERED,
	DISGUISED,
	TAUNTING,
	BONKED,
};

class CBaseEntity;
class CPlayerVariables
{
public:
	CBaseEntity* m_pPredictionPlayer;
	int* m_pPredictionRandomSeed;
	DWORD dwPureReturnAddress;
	DWORD dwFogReturnAddress;
	DWORD dwTimeDemoReturnAddress;
	DWORD dwAddFlag;
	WNDPROC pWndProc;
	HWND hWindow;
	bool m_bShouldUpdateAnimations;
	bool n_bIsUpdatingAnimations;
	bool m_bSelfPredicting;

	bool* bSendPacket;
	int iFakeLagAmount = 14;
	int iCurrentChoke;

	bool bDoubleJumped;
	bool OnGround;

	//menu controls
	bool bAimbot;
	bool bAutoshoot;
	int  iAimbotFov = 0;
	int  iAimbotIgnore;
	bool bBacktracking;
	bool bHitscan;
	bool bSilent;
	bool bEspPlayers;
	bool bEspBuildings;
	bool bEspOther;

	bool bAimIgnores[5] = { false, false, false, false, false };

	bool bName;
	bool bNameBuildings;
	bool bGlow;
	bool bHealth;
	bool bHealthBuildings;
	bool bBox;
	bool bBoxBuildings;
	bool bBuildingLevel;
	bool bWeapon;
	bool bConditions;
	bool bChams;
	bool bXQZ;
	bool bBacktrackChams;
	bool bTeamColored;
	bool bTeammates;
	bool bSkeleton;
	bool bBuildings;
	bool bOther;
	bool bBhop;
	bool bRadar;
	bool bSpecList;
	bool bWatermark;
	bool bAutoStab;
	bool bThirdPerson;
	int  iFov = 90;
	int  iSequenceFreezing = 0;
	int  iRadarSize = 90;
	int  iRadarPosX = 600;
	int  iRadarPosY = 200;

	float clrVisibleChams[3] = { 0.f, 1.f, 0.f };
	float clrXQZChams[3] = { 0.f, 0.f, 1.f };

	float clrVisibleChamsFriendly[3] = { 1.f, 1.f, 0.f };
	float clrXQZChamsFriendly[3] = { 1.f, 0.f, 0.f };
	struct
	{
		bool addflag;
		bool msqt;
		bool velocity;
		bool teleported;
		bool building;
	} Debug;
};

struct CFriends
{
//todo...
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
	ConVar* sv_accelerate;
	ConVar* cl_forwardspeed;
	ConVar* cl_sidespeed;
};

class COffsets
{
public:
	int iCreateMoveOffset = 21, iPaintTraverseOffset = 41;
};

struct CDebug
{
	bool  m_bTargetWasFound;
	int   m_nTargetListSize;
	float m_flAcceleration;
};
extern CDebug gDebug;