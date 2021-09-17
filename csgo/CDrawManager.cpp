#include "CDrawManager.h"
//===================================================================================
CDrawManager gDrawManager;
CScreenSize gScreenSize;

#define ESP_HEIGHT 14
#define MENU_HEIGHT 14

//===================================================================================
std::wstring CDrawManager::StringToWide(const std::string& text)
{
	std::wstring wide(text.length(), L' ');
	std::copy(text.begin(), text.end(), wide.begin());

	return wide;
}

void CDrawManager::Initialize( )
{
	if ( gInts.Surface == NULL )
		return;

	gInts.Engine->GetScreenSize( gScreenSize.iScreenWidth, gScreenSize.iScreenHeight );

	m_Font = gInts.Surface->CreateFont( );
	gInts.Surface->SetFontGlyphSet( m_Font, "Tahoma", MENU_HEIGHT, 800, 0, 0, FONTFLAG_DROPSHADOW);

	m_FontAlt = gInts.Surface->CreateFont( );
	gInts.Surface->SetFontGlyphSet( m_FontAlt, "Courier New", ESP_HEIGHT, NULL, NULL, NULL, FONTFLAG_OUTLINE);
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

unsigned long CDrawManager::GetFont()
{
	return m_Font;
}

unsigned long CDrawManager::GetAltFont()
{
	return m_FontAlt;
}

const char* CDrawManager::GetCurrentWeapon(CBaseEntity* pEntity)
{
	if (!pEntity || !pEntity->GetActiveWeapon())
		return "";

	if (pEntity->GetActiveWeapon()->IsKnife(pEntity->GetActiveWeapon()->GetItemDefinitionIndex()))
	{
		return "knife"; 
	}
	else
	{
		//i couldve used weaponname or something like that from weapon data
		switch (pEntity->GetActiveWeapon()->GetItemDefinitionIndex())
		{
		case WEAPON_AK47:
			return "ak47"; break;
		case WEAPON_AUG:
			return "aug"; break;
		case WEAPON_AWP:
			return "awp"; break;
		case WEAPON_BIZON:
			return "pp-bizon"; break;
		case WEAPON_C4:
			return "c4"; break;
		case WEAPON_CZ75A:
			return "cz75"; break;
		case WEAPON_DEAGLE:
			return "deagle"; break;
		case WEAPON_DECOY:
			return "decoy"; break;
		case WEAPON_ELITE:
			return "dualies"; break;
		case WEAPON_FAMAS:
			return "famas"; break;
		case WEAPON_FIVESEVEN:
			return "five7"; break;
		case WEAPON_FLASHBANG:
			return "flashbang"; break;
		case WEAPON_G3SG1:
			return "g3sg1"; break;
		case WEAPON_GALILAR:
			return "galil"; break;
		case WEAPON_GLOCK:
			return "glock"; break;
		case WEAPON_HEGRENADE:
			return "grenade"; break;
		case WEAPON_INCGRENADE: case WEAPON_MOLOTOV:
			return "molly"; break;
		case WEAPON_M249:
			return "m249"; break;
		case WEAPON_M4A1:
			return "m4a4"; break;
		case WEAPON_M4A1_SILENCER:
			return "m4a1s"; break;
		case WEAPON_MAC10:
			return "mac10"; break;
		case WEAPON_MAG7:
			return "mag7"; break;
		case WEAPON_MP5_SD:
			return "mp5"; break;
		case WEAPON_MP7:
			return "mp7"; break;
		case WEAPON_MP9:
			return "mp9"; break;
		case WEAPON_NEGEV:
			return "negev"; break;
		case WEAPON_NOVA:
			return "nova"; break;
		case WEAPON_HKP2000:
			return "p2k"; break;
		case WEAPON_P250:
			return "p250"; break;
		case WEAPON_P90:
			return "p90"; break;
		case WEAPON_REVOLVER:
			return "revolver"; break;
		case WEAPON_SAWEDOFF:
			return "sawedoff"; break;
		case WEAPON_SCAR20:
			return "scar20"; break;
		case WEAPON_SSG08:
			return "ssg08"; break;
		case WEAPON_SG556:
			return "sg553"; break;
		case WEAPON_SMOKEGRENADE:
			return "smoke"; break;
		case WEAPON_TASER:
			return "taser"; break;
		case WEAPON_TEC9:
			return "tec9"; break;
		case WEAPON_UMP45:
			return "ump45"; break;
		case WEAPON_USP_SILENCER:
			return "usp-s"; break;
		case WEAPON_XM1014:
			return "xm1014"; break;
		}
	}

	return "";
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

	origin = pEntity->GetAbsOrigin();

	pEntity->ComputeHitboxSurroundingBox(&mins, &maxs);

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
