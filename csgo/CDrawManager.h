#pragma once
//===================================================================================
#include "SDK.h"
#include "Panels.h"
#define RED(COLORCODE)	((int) ( COLORCODE >> 24) )
#define BLUE(COLORCODE)	((int) ( COLORCODE >> 8 ) & 0xFF )
#define GREEN(COLORCODE)	((int) ( COLORCODE >> 16 ) & 0xFF )
#define ALPHA(COLORCODE)	((int) COLORCODE & 0xFF )
#define COLORCODE(r,g,b,a)((DWORD)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))
//===================================================================================
class CDrawManager
{
public:
	std::wstring StringToWide(const std::string & text);
	void Initialize( );
	void DrawString(DWORD dwFont, int x, int y, DWORD dwColor, const wchar_t * pszText);
	void DrawString(DWORD dwFont, int x, int y, DWORD dwColor, const char * pszText, ...);
	void DrawStringA(DWORD dwFont, bool center, int x, int y, DWORD dwColor, const char * input, ...);
	unsigned long GetFont();
	unsigned long GetAltFont();
	const char * GetCurrentWeapon(CBaseEntity * pEntity);
	byte GetESPHeight( );
	byte GetMenuHeight();
	void DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius );
	void DrawLine(int x1, int y1, int x2, int y2, DWORD color);
	int GetWidth(DWORD dwFont, const char * input);
	void DrawRect( int x, int y, int w, int h, DWORD dwColor );
	void OutlineRect( int x, int y, int w, int h, DWORD dwColor );
	bool WorldToScreen( Vector &vOrigin, Vector &vScreen );

	bool CalculatePlayerBox(CBaseEntity * pEntity, int & x, int & y, int & w, int & h);

	DWORD dwGetTeamColor( int iIndex )
	{
		static DWORD dwColors[] = { 0, //Dummy
					 0, // 1 Teamone (UNUSED)
					 COLOR_DOD_RED, // 2 Teamtwo (RED)
					 COLOR_DOD_GREEN, // 3 teamthree (BLUE)
					 0 // 4 Teamfour (UNUSED) 
					 };
		return dwColors[ iIndex ];
	}

	/*int GetPing()
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
	}*/

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/vgui_netgraphpanel.cpp#L744
	int GetTickRate()
	{
		return int(1.f / gInts.Globals->interval_per_tick);
	}

	int GetClientInterpAmount()
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
	}

	int GetFPS()
	{
		static float m_Framerate = 0;
		m_Framerate = 0.9 * m_Framerate + (1.0 - 0.9) * gInts.Globals->absoluteFrameTime;

		return int(1.f / m_Framerate);
	}

private:
	unsigned long m_Font;
	unsigned long m_FontAlt;
};
//===================================================================================
extern CDrawManager gDrawManager;
//===================================================================================