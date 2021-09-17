#pragma once
#include "SDK.h"
#include "Math.h"
#include <algorithm>


struct IPlayerData
{
	CBaseEntity* m_pEntity;
	Vector       m_vecHitbox;
	int          m_Index;
	float        m_Fov;
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
	bool IsVisible(CBaseEntity * pLocal, CBaseEntity * pEntity, Vector & src, Vector & dst);
	bool GetHitboxes(CBaseEntity* pLocal, CBaseEntity* pEntity, std::vector<int> &hitboxes);
	bool GetHitboxPoints(CBaseEntity* pEntity, matrix3x4 * bones, int index, std::vector<Vector>& points);
	Vector GetAimPoint(CBaseEntity * pLocal, CBaseEntity * pEntity, CUserCmd * pCommand);
	void GetBestTargets(CBaseEntity * pLocal, CUserCmd * pCommand);
	void SetToUninterpolated(CBaseEntity * pEntity, CBaseEntity * pLocal);
	void Think(CUserCmd * pCommand, CBaseEntity * pLocal);
	matrix3x4 m_Matrix[64][128];

	float GetLerpTime()
	{
		if (!gConvars.cl_updaterate)
		{
			gConvars.cl_interp = gInts.Cvar->FindVar("cl_interp");
			gConvars.cl_updaterate = gInts.Cvar->FindVar("cl_updaterate");
			gConvars.cl_interp_ratio = gInts.Cvar->FindVar("cl_interp_ratio");
			gConvars.sv_minupdaterate = gInts.Cvar->FindVar("sv_minupdaterate");
			gConvars.sv_maxupdaterate = gInts.Cvar->FindVar("sv_maxupdaterate");
		}

		float update_rate = std::clamp(gConvars.cl_updaterate->GetFloat(), gConvars.sv_minupdaterate->GetFloat(), gConvars.sv_maxupdaterate->GetFloat());
		float lerp_ratio = gConvars.cl_interp_ratio->GetFloat();

		if (lerp_ratio == 0.0f)
			lerp_ratio = 1.0f;

		float lerp_amount = gConvars.cl_interp->GetFloat();

		ConVar* pMin = gInts.Cvar->FindVar("sv_client_min_interp_ratio");
		ConVar* pMax = gInts.Cvar->FindVar("sv_client_max_interp_ratio");

		if (pMin && pMax && pMin->GetFloat() != -1.0f)
		{
			lerp_ratio = std::clamp(lerp_ratio, pMin->GetFloat(), pMax->GetFloat());
		}
		else
		{
			if (lerp_ratio == 0.0f)
				lerp_ratio = 1.0f;
		}

		float ret = fmax(lerp_amount, lerp_ratio / update_rate);

		return ret;
	}
		
};
extern CAimbot gAimbot;