#pragma once
#include "SDK.h"
#include "Util.h"

struct IEspPlayerData
{
	int           m_Index;
	CBaseEntity*  m_pEntity;
};

struct IEspBuildingsData
{
	int           m_Index;
	CBaseEntity*  m_pEntity;
};

class CEsp
{
public:
	std::vector<IEspPlayerData> m_Players = { };
	std::vector<IEspBuildingsData> m_Buildings = { };

	void Think(CBaseEntity * pLocal);
	void Conditions(CBaseEntity * pEntity);
	//void Players(CBaseEntity * pLocal);
	void Players(CBaseEntity * pLocal, CBaseEntity * pEntity);
	void Buildings(CBaseEntity * pLocal, CBaseEntity * pEntity);
	void FakeLagIndicator(CBaseEntity * pLocal);

	void SpectatorsList(CBaseEntity * pLocal);

};
extern CEsp gESP;

class CRadar
{
public:
	void DrawRadarPoint(Vector vOriginX, Vector vOriginY, Vector qAngle, CBaseEntity * pBaseEntity, DWORD dwTeamColor);
	void DrawRadarBack();
	void Think(CBaseEntity * pLocal);
private:
	POINT Mouse;
	bool bRadarPos = true;
	int SavedRadarX = 0, SavedRadarY = 0;
};
extern CRadar gRadar;

///congrats if u found this garbage.
/*
void WaterMark()
{
	if (!gPlayerVars.bWatermark)
		return;

	auto GetPing = []() -> int
	{
		float m_AvgLatency = 0.0f;
		float flAdjust = 0.0f;

		CNetChannel* pNetChannel = gInts.Engine->GetNetChannelInfo();
		m_AvgLatency = pNetChannel->GetAvgLatency(FLOW_OUTGOING);


		if (gConvars.cl_updaterate->GetFloat() > 0.001f)
		{
			flAdjust = -0.5f / gConvars.cl_updaterate->GetFloat();
			m_AvgLatency += flAdjust;
		}

		m_AvgLatency = (std::max)(0.0f, m_AvgLatency);

		return (int)(m_AvgLatency * 1000.0f);
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L744
	auto GetTickRate = []() -> int
	{
		return int(1.f / gInts.Globals->interval_per_tick);
	};

	auto GetClientInterpAmount = []() -> int
	{
		float flInterp = 0.0;
		float flLerp = 0.0;

		if (gConvars.cl_updaterate)
		{
			flInterp = (std::max)(gConvars.cl_interp->GetFloat(), gConvars.cl_interp_ratio->GetFloat() / gConvars.cl_updaterate->GetFloat());
		}
		else
		{
			flInterp = 0.1f;
		}

		flLerp = (flInterp * 1000.0f);
		return int(flLerp);
	};

	auto GetFPS = []() -> int
	{
		static float m_Framerate = 0;
		m_Framerate = 0.9 * m_Framerate + (1.0 - 0.9) * gInts.Globals->absoluteframetime;

		return int(1.f / m_Framerate);
	};

	time_t t = std::time(nullptr);
	std::ostringstream time;
	time << std::put_time(std::localtime(&t), ("%T, %a %b %d %Y"));

	int m_width, m_height;
	gInts.Engine->GetScreenSize(m_width, m_height);

	Vector vP = (Vector(20, 20, 0));
	int h = 35;
	int y = vP.y + 2;

	//nitro(?)
	//FF6243

	//shapes
	gDrawManager.DrawRect(vP.x, vP.y - 2, 260, 2, COLORCODE(255, 120, 25, 250));
	gDrawManager.DrawRect(vP.x, vP.y, 260, h, COLORCODE(45, 45, 45, 200));

	//strings
	gDrawManager.DrawString(gDrawManager.GetFont(), vP.x + 3, y, COLORCODE(255, 255, 255, 255), "reDodstorm, %s", time.str().data());
	y += gDrawManager.GetMenuHeight();

	//i wanted to use std::string_view however i would need to switch to c++17
	//and im not sure if i could do that because iirc some stuff used is deprecated/removed.
	std::string strFps = "FPS: " + std::to_string(GetFPS());
	std::string strPing = "PING: " + std::to_string(GetPing());
	std::string strTickrate = "TICK: " + std::to_string(GetTickRate());
	std::string flLerp = "LERP: " + std::to_string(GetClientInterpAmount());
	int iAdjust = vP.x + 3;

	gDrawManager.DrawString(gDrawManager.GetFont(), iAdjust, y, COLORCODE(255, 120, 25, 255), strFps.c_str());
	iAdjust += gDrawManager.GetWidth(gDrawManager.GetFont(), strFps.c_str()) + 8;
	gDrawManager.DrawString(gDrawManager.GetFont(), iAdjust, y, COLORCODE(255, 120, 25, 255), strPing.c_str());
	iAdjust += gDrawManager.GetWidth(gDrawManager.GetFont(), strPing.c_str()) + 8;
	gDrawManager.DrawString(gDrawManager.GetFont(), iAdjust, y, COLORCODE(255, 120, 25, 255), strTickrate.c_str());
	iAdjust += gDrawManager.GetWidth(gDrawManager.GetFont(), strTickrate.c_str()) + 8;
	gDrawManager.DrawString(gDrawManager.GetFont(), iAdjust, y, COLORCODE(255, 120, 25, 255), flLerp.c_str());
	y += gDrawManager.GetMenuHeight();
}
*/