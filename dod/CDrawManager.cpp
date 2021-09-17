#include "CDrawManager.h"
//===================================================================================
CDrawManager gDrawManager;

#define ESP_HEIGHT 13
#define MENU_HEIGHT 14

//===================================================================================
std::wstring CDrawManager::StringToWide(const std::string& text)
{
	std::wstring wide(text.length(), L' ');
	std::copy(text.begin(), text.end(), wide.begin());

	return wide;
}
//===================================================================================
void CDrawManager::Initialize( )
{
	if ( gInts.Surface == NULL )
		return;

	gInts.Engine->GetScreenSize( gScreenSize.iScreenWidth, gScreenSize.iScreenHeight );

	m_Font = gInts.Surface->CreateFont( );
	gInts.Surface->SetFontGlyphSet( m_Font, "Tahoma", MENU_HEIGHT, 800, 0, 0, FONTFLAG_DROPSHADOW);

	m_FontAlt = gInts.Surface->CreateFont( );
	gInts.Surface->SetFontGlyphSet( m_FontAlt, "Tahoma", ESP_HEIGHT, FW_BOLD, NULL, NULL, FONTFLAG_DROPSHADOW );
}
//===================================================================================
void CDrawManager::DrawString( DWORD dwFont, int x, int y, DWORD dwColor, const wchar_t *pszText)
{
	if( pszText == NULL )
		return;

	gInts.Surface->DrawSetTextPos( x, y );
	gInts.Surface->DrawSetTextFont( dwFont );
	gInts.Surface->DrawSetTextColor( RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor) );
	gInts.Surface->DrawPrintText( pszText, wcslen( pszText ) );
}
//===================================================================================
void CDrawManager::DrawString( DWORD dwFont, int x, int y, DWORD dwColor, const char *pszText, ... )
{
	if( pszText == NULL )
		return;

	va_list va_alist;
	char szBuffer[1024] = { '\0' };
	wchar_t szString[1024] = { '\0' };

	va_start( va_alist, pszText );
	vsprintf_s( szBuffer, pszText, va_alist );
	va_end( va_alist );

	wsprintfW( szString, L"%S", szBuffer );

	gInts.Surface->DrawSetTextPos( x, y );
	gInts.Surface->DrawSetTextFont( dwFont );
	gInts.Surface->DrawSetTextColor( RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor) );
	gInts.Surface->DrawPrintText( szString, wcslen( szString ) );
}
void CDrawManager::DrawStringA(DWORD dwFont, bool center, int x, int y, DWORD dwColor, const char *input, ... )
{
	CHAR szBuffer[MAX_PATH];

	if (!input)
		return;

	vsprintf(szBuffer, input, (char*)&input + _INTSIZEOF(input));

	if (center)
		x -= GetWidth(dwFont, szBuffer) / 2;

	gInts.Surface->DrawSetTextColor(RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor));
	gInts.Surface->DrawSetTextFont( dwFont );
	gInts.Surface->DrawSetTextPos(x, y);
	std::wstring wide = StringToWide(std::string(szBuffer));
	gInts.Surface->DrawPrintText(wide.c_str(), wide.length());
}
const char* CDrawManager::GetWeaponType(CBaseEntity* pEntity)
{
	if (pEntity == NULL || pEntity->GetActiveWeapon() == NULL)
		return "";

	DWORD *pDODWpnData = pEntity->GetActiveWeapon()->GetDODWpnData();

	if (!pDODWpnData)
		return "";

	int m_WeaponType = *(int*)(pDODWpnData + 0x1CE);

	if (!m_WeaponType)
		return "";

	switch (m_WeaponType)
	{
	case WPN_TYPE_MELEE:
		return "Melee";
	case WPN_TYPE_CAMERA:
		return "Camera or Bomb IDK"; // DoD:S code at its finest.
	case WPN_TYPE_GRENADE:
		return "Grenade";
	case WPN_TYPE_PISTOL:
		return "Pistol";
	case WPN_TYPE_RIFLE:
		return "Rifle";
	case WPN_TYPE_SNIPER:
		return "Sniper";
	case WPN_TYPE_BAZOOKA:
		return "Bazooka";
	case WPN_TYPE_BANDAGE:
		return "Bandage";
	case WPN_TYPE_MG:
		return "MG";
	case WPN_TYPE_RIFLEGRENADE:
		return "RifleGrenade";
	case WPN_TYPE_SIDEARM:
		return "Sidearm";
	case WPN_TYPE_SUBMG:
		return "SubMG";
	}

	return "Nuck figgers";
}

const char* CDrawManager::szCurrentWeaponName(CBaseEntity* pEntity)
{
	if (pEntity == NULL || pEntity->GetActiveWeapon() == NULL)
		return ""; 

	switch (pEntity->GetActiveWeapon()->GetWeaponID())
	{
	case WEAPON_30CAL: case WEAPON_30CAL_UNDEPLOYED:
		return "30CAL";
	case WEAPON_AMERKNIFE:
		return "Knife";
	case WEAPON_BAR: case WEAPON_BAR_SEMIAUTO:
		return "BAR";
	case WEAPON_BAZOOKA:
		return "Bazooka";
	case WEAPON_C96:
		return "C96";
	case WEAPON_COLT:
		return "M1911";
	case WEAPON_FRAG_GER: case WEAPON_FRAG_GER_LIVE: case WEAPON_FRAG_US: case WEAPON_FRAG_US_LIVE:
		return "Grenade";
	case WEAPON_GARAND: case WEAPON_GARAND_ZOOMED:
		return "Garand";
	case WEAPON_K98: case WEAPON_K98_SCOPED: case WEAPON_K98_SCOPED_ZOOMED: case WEAPON_K98_ZOOMED:
		return "Kar98";
	case WEAPON_M1CARBINE:
		return "M1 Carbine";
	case WEAPON_MG42: case WEAPON_MG42_UNDEPLOYED:
		return "MG42";
	case WEAPON_MP40: case WEAPON_MP40_PUNCH:
		return "MP40";
	case WEAPON_MP44: case WEAPON_MP44_SEMIAUTO:
		return "MP44";
	case WEAPON_P38:
		return "P38";
	case WEAPON_PSCHRECK:
		return "Panzerschreck";
	case WEAPON_RIFLEGREN_GER: case WEAPON_RIFLEGREN_GER_LIVE: case WEAPON_RIFLEGREN_US: case WEAPON_RIFLEGREN_US_LIVE:
		return "Rifle grenade";
	case WEAPON_SMOKE_GER: case WEAPON_SMOKE_US:
		return "Smoke";
	case WEAPON_SPADE:
		return "Shovel";
	case WEAPON_SPRING: case WEAPON_SPRING_ZOOMED:
		return "Springfield";
	case WEAPON_THOMPSON: case WEAPON_THOMPSON_PUNCH:
		return "Thompson";
	}

	return "";
}

unsigned long CDrawManager::GetFont()
{
	return m_Font;
}

unsigned long CDrawManager::GetAltFont()
{
	return m_FontAlt;
}

//===================================================================================
byte CDrawManager::GetESPHeight( )
{
	return ESP_HEIGHT;
}
byte CDrawManager::GetMenuHeight()
{
	return MENU_HEIGHT;
}
//===================================================================================
void CDrawManager::DrawRect( int x, int y, int w, int h, DWORD dwColor )
{
	gInts.Surface->DrawSetColor( RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor) );
	gInts.Surface->DrawFilledRect( x, y, x + w, y + h );
}
//===================================================================================
void CDrawManager::OutlineRect( int x, int y, int w, int h, DWORD dwColor )
{
	gInts.Surface->DrawSetColor( RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor) );
	gInts.Surface->DrawOutlinedRect( x, y, x + w, y + h );
}
//===================================================================================
void CDrawManager::DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius )
{
	Vector vScreen;

	if( !WorldToScreen( vOrigin, vScreen ) )
		return;

	int radius2 = radius << 1;

	OutlineRect( vScreen.x - radius + box_width, vScreen.y - radius + box_width, radius2 - box_width, radius2 - box_width, 0x000000FF );
	OutlineRect( vScreen.x - radius - 1, vScreen.y - radius - 1, radius2 + ( box_width + 2 ), radius2 + ( box_width + 2 ), 0x000000FF );
	DrawRect( vScreen.x - radius + box_width, vScreen.y - radius, radius2 - box_width, box_width,COLORCODE( r, g, b, alpha ));
	DrawRect( vScreen.x - radius, vScreen.y + radius, radius2, box_width,COLORCODE( r, g, b, alpha ));
	DrawRect( vScreen.x - radius, vScreen.y - radius, box_width, radius2,COLORCODE( r, g, b, alpha ));
	DrawRect( vScreen.x + radius, vScreen.y - radius, box_width, radius2 + box_width, COLORCODE( r, g, b, alpha ) );
}
void CDrawManager::DrawLine(int x1, int y1, int x2, int y2, DWORD color)
{
	gInts.Surface->DrawSetColor(RED(color), GREEN(color), BLUE(color), ALPHA(color));
	gInts.Surface->DrawLine(x1, y1, x2, y2);
}
int CDrawManager::GetWidth(DWORD dwFont, const char* input)
{
	INT iWide = 0;
	INT iTall = 0;
	INT iBufSize = MultiByteToWideChar(CP_UTF8, 0x0, input, -1, NULL, 0);

	wchar_t* pszUnicode = new wchar_t[iBufSize];

	MultiByteToWideChar(CP_UTF8, 0x0, input, -1, pszUnicode, iBufSize);

	gInts.Surface->GetTextSize(dwFont, pszUnicode, iWide, iTall);

	delete[] pszUnicode;

	return iWide;
}
//===================================================================================
bool CDrawManager::WorldToScreen( Vector &vOrigin, Vector &vScreen )
{
	return (gInts.DebugOverlay->ScreenPosition(vOrigin, vScreen) != 1);
}


bool CDrawManager::CalculatePlayerBox(CBaseEntity* pEntity, int & x, int & y, int & w, int & h)
{
	Vector origin, mins, maxs;
	Vector bottom, top;

	// get interpolated origin.
	origin = pEntity->GetAbsOrigin();

	// get hitbox bounds.
	pEntity->ComputeHitboxSurroundingBox(&mins, &maxs);

	// correct x and y coordinates.
	mins = { origin.x, origin.y, mins.z };
	maxs = { origin.x, origin.y, maxs.z + 8.f };

	if (!WorldToScreen(mins, bottom) || !WorldToScreen(maxs, top))
		return false;

	h = bottom.y - top.y;
	w = h / 2.f;
	x = bottom.x - (w / 2.f);
	y = bottom.y - h;

	return true;
}

void CDrawManager::CalculateBounds(CBaseEntity * pEntity, int & x, int & y, int & w, int & h)
{
	player_info_t pInfo;
	if (!gInts.Engine->GetPlayerInfo(pEntity->GetIndex(), &pInfo))
		return;

	const matrix3x4& vMatrix = pEntity->GetCoordinateFrame();

	Vector vMin = pEntity->GetCollideableMins();
	Vector vMax = pEntity->GetCollideableMaxs();

	Vector vPointList[] = {
		Vector(vMin.x, vMin.y, vMin.z),
		Vector(vMin.x, vMax.y, vMin.z),
		Vector(vMax.x, vMax.y, vMin.z),
		Vector(vMax.x, vMin.y, vMin.z),
		Vector(vMax.x, vMax.y, vMax.z),
		Vector(vMin.x, vMax.y, vMax.z),
		Vector(vMin.x, vMin.y, vMax.z),
		Vector(vMax.x, vMin.y, vMax.z)
	};

	Vector vTransformed[8];

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 3; j++)
			vTransformed[i][j] = vPointList[i].Dot((Vector&)vMatrix[j]) + vMatrix[j][3];

	Vector flb, brt, blb, frt, frb, brb, blt, flt;

	if (!gDrawManager.WorldToScreen(vTransformed[3], flb) ||
		!gDrawManager.WorldToScreen(vTransformed[0], blb) ||
		!gDrawManager.WorldToScreen(vTransformed[2], frb) ||
		!gDrawManager.WorldToScreen(vTransformed[6], blt) ||
		!gDrawManager.WorldToScreen(vTransformed[5], brt) ||
		!gDrawManager.WorldToScreen(vTransformed[4], frt) ||
		!gDrawManager.WorldToScreen(vTransformed[1], brb) ||
		!gDrawManager.WorldToScreen(vTransformed[7], flt))
		return;

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for (int i = 0; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (top < arr[i].y)
			top = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (bottom > arr[i].y)
			bottom = arr[i].y;
	}

	x = left;
    y = bottom;
	w = right - left;
	h = top - bottom;
}