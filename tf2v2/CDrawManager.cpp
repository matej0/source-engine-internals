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
const char* CDrawManager::szCurrentWeaponName(CBaseEntity* pEntity)
{
	if (pEntity == NULL || pEntity->GetActiveWeapon() == NULL)
		return ""; //This is usually caused by some faggot doing the T-pose glitch.

	int iWeaponID = pEntity->GetActiveWeapon()->GetItemDefinitionIndex(); //AttributeContainer::m_ItemId

	switch (iWeaponID)
	{
		//================================================
		//Scout
		//Primary
	case WPN_FAN: case WPN_FestiveFaN: return "f-a-n";
	case WPN_Shortstop: return "shortstop";
	case WPN_SodaPopper: return "sodapopper";
	case WPN_BabyFaceBlaster: return "babyfaceblaster";
	case WPN_BackScatter: return "backscatter";
		//Secondary
	case WPN_Bonk: return "bonk!";
	case WPN_CritCola: return "crit-a-cola";
	case WPN_Milk: case WPN_MutatedMilk: return "madmilk";
	case WPN_Winger: return "winger";
	case WPN_PocketPistol: return "pocketpistol";
	case WPN_FlyingGuillotine1: case WPN_FlyingGuillotine2: return "flyingguillotine";
		//Melee
	case WPN_Sandman: return "sandman";
	case WPN_Cane: return "candycane";
	case WPN_BostonBasher: case WPN_RuneBlade: return "bostonbasher";
	case WPN_SunStick: return "sunstick";
	case WPN_FanOWar: return "fan o' war";
	case WPN_Atomizer: return "atomizer";
	case WPN_WrapAssassin: return "wrapassassin";
		//================================================
		//Soldier
		//Primary
	case WPN_DirectHit: return "directhit";
	case WPN_Original: return "original"; //This wepaon has special properties over the rocketlauncher in that it fires from the center of view instead of the dominant hand of the player.
	case WPN_BlackBox: case WPN_FestiveBlackbox: return "blackbox";
	case WPN_RocketJumper: return "rocketjumper";
	case WPN_LibertyLauncher: return "libertylauncher";
	case WPN_CowMangler: return "cowmangler";
	case WPN_BeggersBazooka: return "beggersbazooka";
	case WPN_Airstrike: return "airstrike";
		//Secondary
	case WPN_BuffBanner: case WPN_FestiveBuffBanner: return "buffbanner";
	case WPN_BattalionBackup: return "battalionbackup";
	case WPN_Concheror: return "concheror";
	case WPN_ReserveShooter: return "reserveshooter";
	case WPN_RighteousBison: return "righteousbison";
	case WPN_PanicAttack: return "panicattack";
		//Melee
	case WPN_Equalizer: return "equalizer";
	case WPN_EscapePlan: return "escapeplan";
	case WPN_PainTrain: return "paintrain";
	case WPN_Katana: return "katana";
	case WPN_MarketGardener: return "marketgardner";
	case WPN_DisciplinaryAction: return "disciplinaryaction";
		//================================================
		//Pyro
		//Primary
	case WPN_Backburner: case WPN_FestiveBackburner: return "backburner";
	case WPN_Degreaser: return "degreaser";
	case WPN_Phlogistinator: return "phlogistinator";
	case WPN_DragonsFury: return "dragonsfury";
		//Secondary
	case WPN_Flaregun: case WPN_FestiveFlaregun: return "flaregun";
	case WPN_Detonator: return "detonator";
	case WPN_ManMelter: return "manmelter";
	case WPN_ScorchShot: return "scorchshot";
	case WPN_ThermalThruster: return "thermalthruster";
	case WPN_GasPasser: return "gaspasser";
		//Melee
	case WPN_Axtingusher: case WPN_Mailbox: case WPN_FestiveAxtingisher: return "axtinguisher";
	case WPN_HomeWrecker: case WPN_Maul: return "homewrecker";
	case WPN_PowerJack: return "powerjack";
	case WPN_Backscratcher: return "backscratcher";
	case WPN_VolcanoFragment: return "volcanofrag";
	case WPN_ThirdDegree: return "thirddegree";
	case WPN_NeonAnnihilator1: case WPN_NeonAnnihilator2: return "neonannihilator";
	case WPN_HotHand: return "hothand";
		//================================================
		//Demoman
		//Primary
	case WPN_LochNLoad: return "loch-n-load";
	case WPN_LoooseCannon: return "loosecannon";
	case WPN_IronBomber: return "ironbomber";
		//Secondary
	case WPN_ScottishResistance: return "scottishresistance";
	case WPN_StickyJumper: return "stickyjumper";
	case WPN_QuickieBombLauncher: return "quickielauncher";
		//Melee
	case WPN_Sword: case WPN_FestiveEyelander: case WPN_Golfclub: case WPN_Headless: return "sword";
	case WPN_ScottsSkullctter: return "skullcutter";
	case WPN_Fryingpan: return "fryingpan";
	case WPN_Ullapool: return "ullapool";
	case WPN_Claidheamhmor: return "claidheamhmor";
	case WPN_PersainPersuader: return "persainpersuader";
		//================================================
		//Heavy
		//Primary
	case WPN_Natascha: return "natasha";
	case WPN_BrassBeast: return "brassbeast";
	case WPN_Tomislav: return "tomislav";
	case WPN_HuoLongHeatmaker1: case WPN_HuoLongHeatmaker2: return "huolongheater";
		//Secondary
	case WPN_Sandvich: case WPN_RobotSandvich: case WPN_FestiveSandvich: return "sandvich";
	case WPN_Fishcake: case WPN_CandyBar: return "dalokohsbar";
	case WPN_Steak: return "steak";
	case WPN_FamilyBuisness: return "familybusiness";
	case WPN_Banana: return "banana";
		//Melee
	case WPN_KGB: return "kgb";
	case WPN_GRU: case WPN_FestiveGRU: case WPN_BreadBite: return "gru";
	case WPN_WarriorSpirit: return "warriorspirit";
	case WPN_FistsOfSteel: return "fistsofsteel";
	case WPN_EvictionNotice: return "evictionnotice";
	case WPN_ApocoFists: return "apocofists";
	case WPN_HolidayPunch: return "holidaypunch";
		//================================================
		//Engineer
		//Primary
	case WPN_FrontierJustice: case WPN_FestiveFrontierJustice: return "frontierjustice";
	case WPN_Widowmaker: return "widowmaker";
	case WPN_Pomson: return "pomson";
	case WPN_RescueRanger: return "rescueranger";
		//Secondary
	case WPN_Wrangler: case WPN_FestiveWrangler: case WPN_GeigerCounter: return "wrangler";
	case WPN_ShortCircut: return "shortcircut";
		//Melee
	case WPN_Gunslinger: return "robothand";
	case WPN_SouthernHospitality: return "southernhospitality";
	case WPN_Jag: return "jag";
	case WPN_EurekaEffect: return "eurekaeffect";
		//================================================
		//Medic
		//Primary
	case WPN_Blutsauger: return "blutsauger";
	case WPN_Crossbow: case WPN_FestiveCrossbow: return "crossbow";
	case WPN_Overdose: return "overdose";
		//Secondary
	case WPN_Kritzkrieg: return "kritzkrieg";
	case WPN_QuickFix: return "quickfix";
	case WPN_Vaccinator: return "vaccinator";
		//Melee
	case WPN_Ubersaw: case WPN_FestiveUbersaw: return "ubersaw";
	case WPN_Vitasaw: return "vitasaw";
	case WPN_Amputator: return "amputator";
	case WPN_Solemnvow: return "solemnvow";
		//================================================
		//Sniper
		//Primary
	case WPN_Huntsman: case WPN_FestiveHuntsman: case WPN_CompoundBow: return "huntsman";
	case WPN_SydneySleeper: return "sydneysleeper";
	case WPN_Bazaarbargain: return "bazaarbargain";
	case WPN_Machina: case WPN_ShootingStar: return "machina";
	case WPN_HitmanHeatmaker: return "hitmanheatmaker";
	case WPN_ClassicSniperRifle: return "classicsniper";
		//Secondary
	case WPN_Jarate: case WPN_FestiveJarate: case WPN_SelfAwareBeautyMark: return "jarate";
	case WPN_CleanersCarbine: return "cleanerscarbine";
		//Melee
	case WPN_TribalmansShiv: return "tribalsmansshiv";
	case WPN_Bushwacka: return "bushwacka";
	case WPN_Shahanshah: return "shahanshah";
		//================================================
		//Spy
		//Primary
	case WPN_Ambassador: case WPN_FestiveAmbassador: return "ambassador";
	case WPN_Letranger: return "letranger";
	case WPN_Enforcer: return "enforcer";
	case WPN_Diamondback: return "diamondback";
		//Melee
	case WPN_WangaPrick: case WPN_EternalReward: return "eternalreward";
	case WPN_Kunai: return "kunai";
	case WPN_BigEarner: return "bigearner";
	case WPN_Spycicle: return "spycicle";
		//Sapper
	case WPN_RedTape1: case WPN_RedTape2: return "taperecorder";
	default: //For Stock/Unknown weapons
	{
		int iClassID = pEntity->GetClassNum(); //TFPlayerClassShared::m_iClass
		int iSlotID = pEntity->GetActiveWeapon()->GetSlot(); //CBaseCombatWeapon::GetSlot
		switch (iClassID)
		{
		case TF2_Scout:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "scattergun";
			case TF_WEAPONSLOT_SECONDARY:
				return "pistol";
			case TF_WEAPONSLOT_MELEE:
				return "bat";
			}
		}
		case TF2_Soldier:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "rocketlauncher";
			case TF_WEAPONSLOT_SECONDARY:
				return "shotgun";
			case TF_WEAPONSLOT_MELEE:
				return "shovel";
			}
		}
		case TF2_Pyro:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "flamethrower";
			case TF_WEAPONSLOT_SECONDARY:
				return "shotgun";
			case TF_WEAPONSLOT_MELEE:
				return "axe"; //m8 can I axe you a question?
			}
		}
		case TF2_Demoman:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "pipelauncher";
			case TF_WEAPONSLOT_SECONDARY:
				return "stickylauncher";
			case TF_WEAPONSLOT_MELEE:
				return "bottle";
			}
		}
		case TF2_Heavy:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "minigun";
			case TF_WEAPONSLOT_SECONDARY:
				return "shotgun";
			case TF_WEAPONSLOT_MELEE:
				return "fists";
			}
		}
		case TF2_Engineer:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "shotgun";
			case TF_WEAPONSLOT_SECONDARY:
				return "pistol";
			case TF_WEAPONSLOT_MELEE:
				return "wrench";
			case TF_WEAPONSLOT_PDA1:
			case TF_WEAPONSLOT_PDA2:
				return "pda";
			case TF_WEAPONSLOT_BUILDING:
				return "toolbox";
			}
		}
		case TF2_Medic:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "syringegun";
			case TF_WEAPONSLOT_SECONDARY:
				return "medigun";
			case TF_WEAPONSLOT_MELEE:
				return "bonesaw";
			}
		}
		case TF2_Sniper:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "sniperrifle";
			case TF_WEAPONSLOT_SECONDARY:
				return "smg";
			case TF_WEAPONSLOT_MELEE:
				return "kukri";
			}
		}
		case TF2_Spy:
		{
			switch (iSlotID)
			{
			case TF_WEAPONSLOT_PRIMARY:
				return "revolver";
			case TF_WEAPONSLOT_SECONDARY:
				return "sapper";
			case TF_WEAPONSLOT_MELEE:
				return "knife";
			case TF_WEAPONSLOT_PDA1:
				return "disguisekit";
			}
		}
		}
	}
	return "";
	}
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

bool CDrawManager::CalculatePlayerBounds(CBaseEntity* pEntity, int & x, int & y, int & w, int & h)
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

bool CDrawManager::CalculateBounds(CBaseEntity * pEntity, int & x, int & y, int & w, int & h)
{
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
		return false;

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

	return true;
}