#pragma once

#include "SDK.h"

#define WPN_TYPE_MELEE			(1<<0)
#define WPN_TYPE_GRENADE		(1<<1)			
//#define WPN_TYPE_GRENADE_LIVE	(1<<2)	//exploding grenades, unused
#define WPN_TYPE_PISTOL			(1<<3)			
#define WPN_TYPE_RIFLE			(1<<4)			
#define WPN_TYPE_SNIPER			(1<<5)		
#define WPN_TYPE_SUBMG			(1<<6)			
#define WPN_TYPE_MG				(1<<7)	//mg42, 30cal
#define WPN_TYPE_BAZOOKA		(1<<8)
#define WPN_TYPE_BANDAGE		(1<<9)
#define WPN_TYPE_SIDEARM		(1<<10)	//carbine - secondary weapons
#define WPN_TYPE_RIFLEGRENADE	(1<<11)
#define WPN_TYPE_BOMB			(1<<12)
#define WPN_TYPE_UNKNOWN		(1<<13)
#define WPN_TYPE_CAMERA			(1<<12)

#define WPN_MASK_GUN	( WPN_TYPE_PISTOL | WPN_TYPE_RIFLE | WPN_TYPE_SNIPER | WPN_TYPE_SUBMG | WPN_TYPE_MG | WPN_TYPE_SIDEARM )

typedef enum
{
	WEAPON_NONE = 0,

	//Melee
	WEAPON_AMERKNIFE,
	WEAPON_SPADE,

	//Pistols
	WEAPON_COLT,
	WEAPON_P38,
	WEAPON_C96,

	//Rifles
	WEAPON_GARAND,
	WEAPON_M1CARBINE,
	WEAPON_K98,

	//Sniper Rifles
	WEAPON_SPRING,
	WEAPON_K98_SCOPED,

	//SMG
	WEAPON_THOMPSON,
	WEAPON_MP40,
	WEAPON_MP44,
	WEAPON_BAR,

	//Machine guns
	WEAPON_30CAL,
	WEAPON_MG42,

	//Rocket weapons
	WEAPON_BAZOOKA,
	WEAPON_PSCHRECK,

	//Grenades
	WEAPON_FRAG_US,
	WEAPON_FRAG_GER,

	WEAPON_FRAG_US_LIVE,
	WEAPON_FRAG_GER_LIVE,

	WEAPON_SMOKE_US,
	WEAPON_SMOKE_GER,

	WEAPON_RIFLEGREN_US,
	WEAPON_RIFLEGREN_GER,

	WEAPON_RIFLEGREN_US_LIVE,
	WEAPON_RIFLEGREN_GER_LIVE,

	// not actually separate weapons, but defines used in stats recording
	// find a better way to do this without polluting the list of actual weapons.
	WEAPON_THOMPSON_PUNCH,
	WEAPON_MP40_PUNCH,

	WEAPON_GARAND_ZOOMED,
	WEAPON_K98_ZOOMED,
	WEAPON_SPRING_ZOOMED,
	WEAPON_K98_SCOPED_ZOOMED,

	WEAPON_30CAL_UNDEPLOYED,
	WEAPON_MG42_UNDEPLOYED,

	WEAPON_BAR_SEMIAUTO,
	WEAPON_MP44_SEMIAUTO,

	WEAPON_MAX,		// number of weapons weapon index

} DODWeaponID;