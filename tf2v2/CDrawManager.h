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
	const char * szCurrentWeaponName(CBaseEntity * pEntity);
	unsigned long GetFont();
	unsigned long GetAltFont();
	byte GetESPHeight( );
	byte GetMenuHeight();
	int GetPixelTextSize ( const char *pszText );
	int GetPixelTextSize ( wchar_t *pszText );
	void DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius );
	void DrawLine(int x1, int y1, int x2, int y2, DWORD color);
	int GetWidth(DWORD dwFont, const char * input);
	void DrawRect( int x, int y, int w, int h, DWORD dwColor );
	void OutlineRect( int x, int y, int w, int h, DWORD dwColor );
	bool CalculateBounds(CBaseEntity * pEntity, int & x, int & y, int & w, int & h);
	bool WorldToScreen( Vector &vOrigin, Vector &vScreen );

	bool CalculatePlayerBounds(CBaseEntity * pEntity, int & x, int & y, int & w, int & h);

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

private:
	unsigned long m_Font;
	unsigned long m_FontAlt;
};
//===================================================================================
extern CDrawManager gDrawManager;
//===================================================================================