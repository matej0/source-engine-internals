#pragma once
#include "SDK.h"
#include "Util.h"
#include <deque>

//https://www.unknowncheats.me/forum/counterstrike-global-offensive/243848-heavily-optimizing-aimbot.html
//thanks

struct IPlayerData
{
	CBaseEntity* m_pEntity;
	Vector       m_vecHitbox;
	int          m_Index;
	float        m_Fov;
};

struct ILagData
{
	int          m_nIndex;
	float        m_flSimulationTime;
	float        m_Fov;
	bool         m_bValidMatrix;
	Vector       m_vecEyePosition;
	Vector       m_vecEyeAngles;
	Vector       m_vecOrigin;
	matrix3x4    m_pMatrix[128];
	CBaseEntity* m_pEntity;

	bool IsRecordOutOfBounds()
	{
		//thanks polak
		float correct = 0;
		static auto sv_maxunlag = gInts.Cvar->FindVar("sv_maxunlag");

		correct += gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
		correct += gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
		correct += UTIL_GetLerpTime();

		correct = clamp(correct, 0.f, sv_maxunlag->GetFloat());

		float deltaTime = correct - (gInts.Globals->curtime - this->m_flSimulationTime);

		if (fabsf(deltaTime) > 0.2f)
		{
			return true;
		}

		return false;
	}

};

struct ISort
{
	bool operator()(IPlayerData a, IPlayerData b) const
	{
		return a.m_Fov < b.m_Fov;
	}
};
extern ISort gTargetSort;

class CAimbot
{
public:
	void Init();
	Vector GetBestHitbox(CBaseEntity * pLocal, CBaseEntity * pEntity, CUserCmd * pCmd, matrix3x4 * pMatrix);
	void FindTargets(CBaseEntity* pLocal, CUserCmd* pCmd);
	void Think(CBaseEntity* pLocal, CUserCmd* pCmd);
	ILagData * GetLastRecord(int nIndex);
	bool IsNetworkingUpdate(CBaseEntity * pEntity);
	void StoreRecord(CBaseEntity * pLocal, int i);
	void AutoBackstab(CBaseEntity * pLocal, CUserCmd * pCmd);

	std::vector<IPlayerData> m_Targets     = { }; //store all targets in a list.
	std::vector<ILagData>    m_Records[65] = { }; //backtrack records

	matrix3x4 m_Matrix[64][128];
};
extern CAimbot gAim;

class CEnginePrediction
{
public:
	void SetPredictionRandomSeed(CUserCmd * pCmd);
	void SetPredictionPlayer(CBaseEntity * pPlayer);
	void StartCommand(CBaseEntity * pLocal, CUserCmd * pCmd);
	void FinishCommand(CBaseEntity * pLocal);
	void RunPreThink(CBaseEntity * pLocal);
	void RunThink(CBaseEntity * pLocal);
	void RunPostThink(CBaseEntity * pLocal);
	void Start(CBaseEntity * pLocal, CUserCmd * pCmd);
	void End(CBaseEntity * pLocal);
	void Start1(CBaseEntity * pLocal, CUserCmd * pCmd);
	void End1(CBaseEntity * pLocal);
	float m_flOldCurtime;
	float m_flOldFrameTime;
};
extern CEnginePrediction gEnginePred;
