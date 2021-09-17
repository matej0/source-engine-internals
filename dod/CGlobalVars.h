#include "SDK.h"

enum AntiAimSides
{
	AALEFT = -1,
	AARIGHT = 1,
};

class CNetChannel;
class CPlayerVariables
{
public:
	DWORD dwFogReturnAddress;
	bool* bSendPacket;
	int iFakeLagAmount = 14;
	int iCurrentChoke;
	CNetChannel* pNetChannel;
	int* iAASide;

	//menu controls
	bool bAimbot;
	bool bNoRecoil;
	bool bNoSpread;
	bool bName;
	bool bHealth;
	bool bBox;
	bool bWeapon;
	bool bXQZ;
	bool bTeammates;
	bool bSkeleton;
	bool bChams;
	bool bGodMode;
	int iFov = 90;
	int iViewModelFov = 80;
	int iSequenceFreezing = 0;
	int iTest = 0;
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
	ConVar* sv_showimpacts;
	ConVar* r_viewmodelfov;
};

class COffsets
{
public:
	int iCreateMoveOffset = 21, iPaintTraverseOffset = 41;
};