#pragma once
#include <windows.h>
#include <math.h>
#include <xstring>
#include "CUtlVector.h"
#include "Vector.h"
#include "getvfunc.h"
#include "dt_recv2.h"
#include "CSignature.h"
#include "WeaponList.h"
#include "ConVar.h"
#include "CGlobalVars.h"
#include "Log.h"
#include "minhook/minhook.h"
#include "NetVar.h"
#include "studio.h"
#include <string>
#include <ctime>
#include <thread>
#include "MD5.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>
#include <array>
#include <winerror.h>
#include <ShlObj.h>
#include <Psapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdexcept>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

//using namespace std;

#pragma optimize("gsy",on)
#pragma warning( disable : 4244 ) //Possible loss of data

typedef void* ( __cdecl* CreateInterface_t )( const char*, int* );
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
typedef float matrix3x4[3][4];
class CGameTrace;
typedef CGameTrace trace_t;

#define streql(x,y) strcmp( (x) , (y) ) == 0
#define	IMPORT(DLL, FUNC) GetProcAddress(GetModuleHandle(DLL), FUNC)
#define MAX_PLAYER_NAME_LENGTH		32
#define ENABLE_INTERPOLATION 1
#define DISABLE_INTERPOLATION 0
#define DOD_SNIPER_ZOOM_CHANGE_TIME 1
#define WIN32_LEAN_AND_MEAN
#define TF_MAX_SPEED   (400 * 1.3)

//PASTING FROM VALVE!!!!
//#define COLOR_DOD_GREEN	COLORCODE(184, 56, 59, 255) 
//#define COLOR_DOD_RED	COLORCODE(88, 133, 162, 255) 
#define COLOR_DOD_GREEN	COLORCODE(255, 64, 64, 255) 
#define COLOR_DOD_RED	COLORCODE(0, 180, 255, 255) 
#define COLOR_WHITE     COLORCODE(255, 255, 255, 255)

#define me gInts.Engine->GetLocalPlayer()
#define GetBaseEntity gInts.EntList->GetClientEntity
#define MASK_AIMBOT 0x200400B
#define	FL_ONGROUND (1<<0)
#define FL_DUCKING (1<<1)
#define CONTENTS_HITBOX 0x40000000
#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define WEHRMACHT 3
#define AXIS 2

#define	DECLVHOOK(RET, NAME, ARGS)\
	typedef RET (__stdcall* NAME##Fn) ARGS;\
	NAME##Fn o##NAME;\
	RET __stdcall Hooked_ ## NAME ARGS\

template<typename T> inline T get_SI()
{
	__asm mov eax, esi // asm("mov eax, esi");
}


template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

typedef struct player_info_s
{
	char			name[32];
	int				userID;
	char			guid[33];
	unsigned long	friendsID;
	char			friendsName[32];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customFiles[4];
	unsigned char	filesDownloaded;
} player_info_t;

class ClientClass
{
private:
	BYTE _chPadding[8];
public:
	char* chName;
	RecvTable* Table;
	ClientClass* pNextClass;
	int iClassID;
};

class CBoneAccessor
{
public:
	matrix3x4* GetBoneArrayForWrite()
	{
		return m_pBones;
	}

	void SetBoneArrayForWrite(matrix3x4* pBoneArray)
	{
		m_pBones = pBoneArray;
	}

	int GetReadableBones()
	{
		return m_ReadableBones;
	}

	void SetReadableBones(int iFlags)
	{
		m_ReadableBones = iFlags;
	}

	int GetWritableBones()
	{
		return m_WritableBones;
	}

	void SetWritableBones(int iFlags)
	{
		m_WritableBones = iFlags;
	}

	alignas(16) matrix3x4 *m_pBones;
	int m_ReadableBones;		// Which bones can be read.
	int m_WritableBones;		// Which bones can be written.
};

class CHLClient
{
public:
	ClientClass* GetAllClasses( void )
	{
		typedef ClientClass* ( __thiscall* OriginalFn )( PVOID ); //Anything inside a VTable is a __thiscall unless it completly disregards the thisptr. You can also call them as __stdcalls, but you won't have access to the __thisptr.
		return getvfunc<OriginalFn>( this, 8 )( this ); //Return the pointer to the head CClientClass.
	}
};

enum ObserverMode_t
{
	OBS_NONE = 0,			// Self-explanatory.
	OBS_DEATHCAM,			// Not sure what this is, but it's here.
	OBS_FREEZECAME,			// Get killed by someone, you zoom in on them.
	OBS_FIXED,				// From a camera placed in the map.
	OBS_FIRST_PERSON,		// Following the player's view in first-person.
	OBS_THIRD_PERSON,		// Following the player's view in third-person.
	OBS_POINT_OF_INTEREST,  // If you are the point of interest, boom.
	OBS_FREE_ROAMING,		// Just zoomin' around.
};

enum
{
	EFL_KILLME = (1 << 0),	// This entity is marked for death -- This allows the game to actually delete ents at a safe time
	EFL_DORMANT = (1 << 1),	// Entity is dormant, no updates to client
	EFL_NOCLIP_ACTIVE = (1 << 2),	// Lets us know when the noclip command is active.
	EFL_SETTING_UP_BONES = (1 << 3),	// Set while a model is setting up its bones.
	EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4), // This is a special entity that should not be deleted when we restart entities only

	EFL_HAS_PLAYER_CHILD = (1 << 4),	// One of the child entities is a player.

	EFL_DIRTY_SHADOWUPDATE = (1 << 5),	// Client only- need shadow manager to update the shadow...
	EFL_NOTIFY = (1 << 6),	// Another entity is watching events on this entity (used by teleport)

	// The default behavior in ShouldTransmit is to not send an entity if it doesn't
	// have a model. Certain entities want to be sent anyway because all the drawing logic
	// is in the client DLL. They can set this flag and the engine will transmit them even
	// if they don't have a model.
	EFL_FORCE_CHECK_TRANSMIT = (1 << 7),

	EFL_BOT_FROZEN = (1 << 8),	// This is set on bots that are frozen.
	EFL_SERVER_ONLY = (1 << 9),	// Non-networked entity.
	EFL_NO_AUTO_EDICT_ATTACH = (1 << 10), // Don't attach the edict; we're doing it explicitly

	// Some dirty bits with respect to abs computations
	EFL_DIRTY_ABSTRANSFORM = (1 << 11),
	EFL_DIRTY_ABSVELOCITY = (1 << 12),
	EFL_DIRTY_ABSANGVELOCITY = (1 << 13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
	EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),
	//	UNUSED						= (1<<16),

	EFL_IN_SKYBOX = (1 << 17),	// This is set if the entity detects that it's in the skybox.
											// This forces it to pass the "in PVS" for transmission.
											EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),	// Entities with this flag set show up in the partition even when not solid
											EFL_TOUCHING_FLUID = (1 << 19),	// Used to determine if an entity is floating

											// FIXME: Not really sure where I should add this...
											EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
											EFL_NO_ROTORWASH_PUSH = (1 << 21),		// I shouldn't be pushed by the rotorwash
											EFL_NO_THINK_FUNCTION = (1 << 22),
											EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),

											EFL_CHECK_UNTOUCH = (1 << 24),
											EFL_DONTBLOCKLOS = (1 << 25),		// I shouldn't block NPC line-of-sight
											EFL_DONTWALKON = (1 << 26),		// NPC;s should not walk on this entity
											EFL_NO_DISSOLVE = (1 << 27),		// These guys shouldn't dissolve
											EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),	// Mega physcannon can't ragdoll these guys.
											EFL_NO_WATER_VELOCITY_CHANGE = (1 << 29),	// Don't adjust this entity's velocity when transitioning into water
											EFL_NO_PHYSCANNON_INTERACTION = (1 << 30),	// Physcannon can't pick these up or punt them
											EFL_NO_DAMAGE_FORCES = (1 << 31),	// Doesn't accept forces from physics damage
};

enum FontFlags 
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0X001,
	FONTFLAG_UNDERLINE = 0X002,
	FONTFLAG_STRIKEOUT = 0X004,
	FONTFLAG_SYMBOL = 0X008,
	FONTFLAG_ANTIALIAS = 0X010,
	FONTFLAG_GAUSSIANBLUR = 0X020,
	FONTFLAG_ROTARY = 0X040,
	FONTFLAG_DROPSHADOW = 0X080,
	FONTFLAG_ADDITIVE = 0X100,
	FONTFLAG_OUTLINE = 0X200,
	FONTFLAG_CUSTOM = 0X400,
	FONTFLAG_BITMAP = 0X800,
};


enum ClassID
{
	CTFWearableRazorback = 340,
	CTFWearableDemoShield = 337,
	CTFWearableLevelableItem = 339,
	CTFWearableCampaignItem = 336,
	CTFBaseRocket = 184,
	CTFWeaponBaseMerasmusGrenade = 324,
	CTFWeaponBaseMelee = 323,
	CTFWeaponBaseGun = 322,
	CTFWeaponBaseGrenadeProj = 321,
	CTFWeaponBase = 320,
	CTFWearableRobotArm = 341,
	CTFRobotArm = 286,
	CTFWrench = 343,
	CTFProjectile_ThrowableBreadMonster = 278,
	CTFProjectile_ThrowableBrick = 279,
	CTFProjectile_ThrowableRepel = 280,
	CTFProjectile_Throwable = 277,
	CTFThrowable = 318,
	CTFSyringeGun = 314,
	CTFKatana = 224,
	CTFSword = 313,
	CSniperDot = 117,
	CTFSniperRifleClassic = 307,
	CTFSniperRifleDecap = 308,
	CTFSniperRifle = 306,
	CTFChargedSMG = 196,
	CTFSMG = 305,
	CTFSlap = 304,
	CTFShovel = 303,
	CTFShotgunBuildingRescue = 302,
	CTFPEPBrawlerBlaster = 240,
	CTFSodaPopper = 309,
	CTFShotgun_Revenge = 300,
	CTFScatterGun = 296,
	CTFShotgun_Pyro = 299,
	CTFShotgun_HWG = 298,
	CTFShotgun_Soldier = 301,
	CTFShotgun = 297,
	CTFRocketPack = 295,
	CTFCrossbow = 200,
	CTFRocketLauncher_Mortar = 294,
	CTFRocketLauncher_AirStrike = 292,
	CTFRocketLauncher_DirectHit = 293,
	CTFRocketLauncher = 291,
	CTFRevolver = 285,
	CTFDRGPomson = 201,
	CTFRaygun = 283,
	CTFPistol_ScoutSecondary = 245,
	CTFPistol_ScoutPrimary = 244,
	CTFPistol_Scout = 243,
	CTFPistol = 242,
	CTFPipebombLauncher = 241,
	CTFWeaponPDA_Spy = 331,
	CTFWeaponPDA_Engineer_Destroy = 330,
	CTFWeaponPDA_Engineer_Build = 329,
	CTFWeaponPDAExpansion_Teleporter = 333,
	CTFWeaponPDAExpansion_Dispenser = 332,
	CTFWeaponPDA = 328,
	CTFParticleCannon = 238,
	CTFParachute_Secondary = 237,
	CTFParachute_Primary = 236,
	CTFParachute = 235,
	CTFMinigun = 233,
	CTFMedigunShield = 230,
	CWeaponMedigun = 351,
	CTFProjectile_MechanicalArmOrb = 262,
	CTFMechanicalArm = 229,
	CTFLunchBox_Drink = 228,
	CTFLunchBox = 227,
	CLaserDot = 78,
	CTFLaserPointer = 226,
	CTFKnife = 225,
	CTFGasManager = 211,
	CTFProjectile_JarGas = 260,
	CTFJarGas = 222,
	CTFProjectile_Cleaver = 253,
	CTFProjectile_JarMilk = 261,
	CTFProjectile_Jar = 259,
	CTFCleaver = 197,
	CTFJarMilk = 223,
	CTFJar = 221,
	CTFWeaponInvis = 327,
	CTFCannon = 195,
	CTFGrenadeLauncher = 215,
	CTFGrenadePipebombProjectile = 216,
	CTFGrapplingHook = 214,
	CTFFlareGun_Revenge = 209,
	CTFFlareGun = 208,
	CTFFlameRocket = 206,
	CTFFlameThrower = 207,
	CTFFists = 204,
	CTFFireAxe = 203,
	CTFWeaponFlameBall = 326,
	CTFCompoundBow = 199,
	CTFClub = 198,
	CTFBuffItem = 194,
	CTFStickBomb = 311,
	CTFBreakableSign = 193,
	CTFBottle = 191,
	CTFBreakableMelee = 192,
	CTFBonesaw = 189,
	CTFBall_Ornament = 181,
	CTFStunBall = 312,
	CTFBat_Giftwrap = 187,
	CTFBat_Wood = 188,
	CTFBat_Fish = 186,
	CTFBat = 185,
	CTFProjectile_EnergyRing = 255,
	CTFDroppedWeapon = 202,
	CTFWeaponSapper = 334,
	CTFWeaponBuilder = 325,
	C_TFWeaponBuilder = 0,
	CTFProjectile_Rocket = 263,
	CTFProjectile_Flare = 256,
	CTFProjectile_EnergyBall = 254,
	CTFProjectile_GrapplingHook = 257,
	CTFProjectile_HealingBolt = 258,
	CTFProjectile_Arrow = 251,
	CMannVsMachineStats = 80,
	CTFTankBoss = 315,
	CTFBaseBoss = 182,
	CBossAlpha = 0,
	NextBotCombatCharacter = 356,
	CTFProjectile_SpellKartBats = 267,
	CTFProjectile_SpellKartOrb = 268,
	CTFHellZap = 219,
	CTFProjectile_SpellLightningOrb = 269,
	CTFProjectile_SpellTransposeTeleport = 276,
	CTFProjectile_SpellMeteorShower = 270,
	CTFProjectile_SpellSpawnBoss = 273,
	CTFProjectile_SpellMirv = 271,
	CTFProjectile_SpellPumpkin = 272,
	CTFProjectile_SpellSpawnHorde = 274,
	CTFProjectile_SpellSpawnZombie = 275,
	CTFProjectile_SpellBats = 265,
	CTFProjectile_SpellFireball = 266,
	CTFSpellBook = 310,
	CHightower_TeleportVortex = 74,
	CTeleportVortex = 159,
	CZombie = 353,
	CMerasmusDancer = 83,
	CMerasmus = 82,
	CHeadlessHatman = 73,
	CEyeballBoss = 48,
	CTFBotHintEngineerNest = 190,
	CBotNPCMinion = 0,
	CBotNPC = 0,
	CPasstimeGun = 94,
	CTFViewModel = 319,
	CRobotDispenser = 111,
	CTFRobotDestruction_Robot = 287,
	CTFReviveMarker = 284,
	CTFPumpkinBomb = 281,
	CTFProjectile_BallOfFire = 252,
	CTFBaseProjectile = 183,
	CTFPointManager = 249,
	CBaseObjectUpgrade = 11,
	CTFRobotDestructionLogic = 290,
	CTFRobotDestruction_RobotGroup = 288,
	CTFRobotDestruction_RobotSpawn = 289,
	CTFPlayerDestructionLogic = 247,
	CPlayerDestructionDispenser = 101,
	CTFMinigameLogic = 232,
	CTFHalloweenMinigame_FallingPlatforms = 218,
	CTFHalloweenMinigame = 217,
	CTFMiniGame = 231,
	CTFPowerupBottle = 250,
	CTFItem = 220,
	CHalloweenSoulPack = 71,
	CTFGenericBomb = 212,
	CBonusRoundLogic = 23,
	CTFGameRulesProxy = 210,
	CTETFParticleEffect = 178,
	CTETFExplosion = 177,
	CTETFBlood = 176,
	CTFFlameManager = 205,
	CHalloweenGiftPickup = 69,
	CBonusDuckPickup = 21,
	CHalloweenPickup = 70,
	CCaptureFlagReturnIcon = 27,
	CCaptureFlag = 26,
	CBonusPack = 22,
	CTFTeam = 317,
	CTFTauntProp = 316,
	CTFPlayerResource = 248,
	CTFPlayer = 246,
	CTFRagdoll = 282,
	CTEPlayerAnimEvent = 164,
	CTFPasstimeLogic = 239,
	CPasstimeBall = 93,
	CTFObjectiveResource = 234,
	CTFGlow = 213,
	CTEFireBullets = 151,
	CTFBuffBanner = 0,
	CTFAmmoPack = 180,
	CObjectTeleporter = 89,
	CObjectSentrygun = 88,
	CTFProjectile_SentryRocket = 264,
	CObjectSapper = 87,
	CObjectCartDispenser = 85,
	CObjectDispenser = 86,
	CMonsterResource = 84,
	CFuncRespawnRoomVisualizer = 64,
	CFuncRespawnRoom = 63,
	CFuncPasstimeGoal = 61,
	CFuncForceField = 57,
	CCaptureZone = 28,
	CCurrencyPack = 31,
	CBaseObject = 10,
	CTestTraceline = 175,
	CTEWorldDecal = 179,
	CTESpriteSpray = 173,
	CTESprite = 172,
	CTESparks = 171,
	CTESmoke = 170,
	CTEShowLine = 168,
	CTEProjectedDecal = 166,
	CTEPlayerDecal = 165,
	CTEPhysicsProp = 163,
	CTEParticleSystem = 162,
	CTEMuzzleFlash = 161,
	CTELargeFunnel = 158,
	CTEKillPlayerAttachments = 157,
	CTEImpact = 156,
	CTEGlowSprite = 155,
	CTEShatterSurface = 167,
	CTEFootprintDecal = 153,
	CTEFizz = 152,
	CTEExplosion = 150,
	CTEEnergySplash = 149,
	CTEEffectDispatch = 148,
	CTEDynamicLight = 147,
	CTEDecal = 145,
	CTEClientProjectile = 144,
	CTEBubbleTrail = 143,
	CTEBubbles = 142,
	CTEBSPDecal = 141,
	CTEBreakModel = 140,
	CTEBloodStream = 139,
	CTEBloodSprite = 138,
	CTEBeamSpline = 137,
	CTEBeamRingPoint = 136,
	CTEBeamRing = 135,
	CTEBeamPoints = 134,
	CTEBeamLaser = 133,
	CTEBeamFollow = 132,
	CTEBeamEnts = 131,
	CTEBeamEntPoint = 130,
	CTEBaseBeam = 129,
	CTEArmorRicochet = 128,
	CTEMetalSparks = 160,
	CSteamJet = 122,
	CSmokeStack = 116,
	DustTrail = 354,
	CFireTrail = 50,
	SporeTrail = 361,
	SporeExplosion = 360,
	RocketTrail = 358,
	SmokeTrail = 359,
	CPropVehicleDriveable = 107,
	ParticleSmokeGrenade = 357,
	CParticleFire = 90,
	MovieExplosion = 355,
	CTEGaussExplosion = 154,
	CEnvQuadraticBeam = 43,
	CEmbers = 36,
	CEnvWind = 47,
	CPrecipitation = 106,
	CBaseTempEntity = 17,
	CWeaponIFMSteadyCam = 350,
	CWeaponIFMBaseCamera = 349,
	CWeaponIFMBase = 348,
	CTFWearableVM = 342,
	CTFWearable = 335,
	CTFWearableItem = 338,
	CEconWearable = 35,
	CBaseAttributableItem = 3,
	CEconEntity = 34,
	CHandleTest = 72,
	CTeamplayRoundBasedRulesProxy = 125,
	CTeamRoundTimer = 126,
	CSpriteTrail = 121,
	CSpriteOriented = 120,
	CSprite = 119,
	CRagdollPropAttached = 110,
	CRagdollProp = 109,
	CPoseController = 105,
	CGameRulesProxy = 68,
	CInfoLadderDismount = 75,
	CFuncLadder = 58,
	CEnvDetailController = 40,
	CWorld = 352,
	CWaterLODControl = 347,
	CWaterBullet = 346,
	CVoteController = 345,
	CVGuiScreen = 344,
	CPropJeep = 0,
	CPropVehicleChoreoGeneric = 0,
	CTest_ProxyToggle_Networkable = 174,
	CTesla = 169,
	CTeamTrainWatcher = 127,
	CBaseTeamObjectiveResource = 16,
	CTeam = 124,
	CSun = 123,
	CParticlePerformanceMonitor = 91,
	CSpotlightEnd = 118,
	CSlideshowDisplay = 115,
	CShadowControl = 114,
	CSceneEntity = 113,
	CRopeKeyframe = 112,
	CRagdollManager = 108,
	CPhysicsPropMultiplayer = 98,
	CPhysBoxMultiplayer = 96,
	CBasePropDoor = 15,
	CDynamicProp = 33,
	CPointCommentaryNode = 104,
	CPointCamera = 103,
	CPlayerResource = 102,
	CPlasma = 100,
	CPhysMagnet = 99,
	CPhysicsProp = 97,
	CPhysBox = 95,
	CParticleSystem = 92,
	CMaterialModifyControl = 81,
	CLightGlow = 79,
	CInfoOverlayAccessor = 77,
	CFuncTrackTrain = 67,
	CFuncSmokeVolume = 66,
	CFuncRotating = 65,
	CFuncReflectiveGlass = 62,
	CFuncOccluder = 60,
	CFuncMonitor = 59,
	CFunc_LOD = 54,
	CTEDust = 146,
	CFunc_Dust = 53,
	CFuncConveyor = 56,
	CBreakableSurface = 25,
	CFuncAreaPortalWindow = 55,
	CFish = 51,
	CEntityFlame = 38,
	CFireSmoke = 49,
	CEnvTonemapController = 46,
	CEnvScreenEffect = 44,
	CEnvScreenOverlay = 45,
	CEnvProjectedTexture = 42,
	CEnvParticleScript = 41,
	CFogController = 52,
	CEntityParticleTrail = 39,
	CEntityDissolve = 37,
	CDynamicLight = 32,
	CColorCorrectionVolume = 30,
	CColorCorrection = 29,
	CBreakableProp = 24,
	CBasePlayer = 13,
	CBaseFlex = 8,
	CBaseEntityEx = 7,
	CBaseDoor = 6,
	CBaseCombatCharacter = 4,
	CBaseAnimatingOverlay = 2,
	CBoneFollower = 20,
	CBaseAnimating = 1,
	CInfoLightingRelative = 76,
	CAI_BaseNPC = 0,
	CBeam = 19,
	CBaseViewModel = 18,
	CBaseProjectile = 14,
	CBaseParticleEntity = 12,
	CBaseGrenade = 9,
	CBaseCombatWeaponEx = 5,
};
class CGlobals
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxclients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
};

class CDebugOverlay
{
public:
	bool ScreenPosition(const Vector& vIn, Vector& vOut)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, const Vector&, Vector&);
		return getvfunc<OriginalFn>(this, 10)(this, vIn, vOut);
	}
};

class CUserCmd
{
public:
	virtual ~CUserCmd() {}; //Destructor 0
	int command_number; //4
	int tick_count; //8
	Vector viewangles; //C
	float forwardmove; //18
	float sidemove; //1C
	float upmove; //20
	int	buttons; //24
	byte impulse; //28
	int weaponselect; //2C
	int weaponsubtype; //30
	int random_seed; //34
	short mousedx; //38
	short mousedy; //3A
	bool hasbeenpredicted; //3C;
};

struct friendsstruct_t
{
	DWORD padding[4];
};

class SteamFriends
{
public:
	const char *GetPersonaName(void)
	{
		typedef const char* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 0)(this);
	}
	bool HasFriend(friendsstruct_t param1)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, friendsstruct_t, int);
		return getvfunc<OriginalFn>(this, 11)(this, param1, 4);
	}
};

class C_AnimationLayer
{
public:

};

enum thinkmethods_t
{
	THINK_FIRE_ALL_FUNCTIONS,
	THINK_FIRE_BASE_ONLY,
	THINK_FIRE_ALL_BUT_BASE,
};


class CBaseCombatWeapon;
class CBaseEntity
{
private:
	template<class T>
	T GetPointer(const int offset)
	{
		return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
	}
	// To get value from the pointer itself
	template<class T>
	T GetValue(const int offset)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
	}

	template <typename T>
	void SetValue(PVOID base, T val)
	{
		*reinterpret_cast<T *>((DWORD)(base)+((DWORD)(off))) = val;
	}
public:
	Vector& GetAbsOrigin( )
	{
		typedef Vector& ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>(this, 9)(this);
	}
	Vector& GetAbsAngles( )
	{
		typedef Vector& ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>(this, 10)(this);
	}

	Vector& GetOldOrigin()
	{
		typedef Vector& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 85)(this);
	}

	void GetWorldSpaceCenter( Vector& vWorldSpaceCenter)
	{
		Vector vMin, vMax;
		this->GetRenderBounds( vMin, vMax );
		vWorldSpaceCenter = this->GetAbsOrigin();
		vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2;
	}
	DWORD* GetModel( )
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef DWORD* ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( pRenderable, 9 )( pRenderable );
	}
	bool SetupBones( matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime )
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef bool ( __thiscall* OriginalFn )( PVOID, matrix3x4*, int, int, float );
		return getvfunc<OriginalFn>( pRenderable, 16 )( pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime );
	}
	ClientClass* GetClientClass( )
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef ClientClass* ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( pNetworkable, 2 )( pNetworkable );
	}
	bool IsDormant( )
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( pNetworkable, 8 )( pNetworkable );
	}
	int GetIndex()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 9)(pNetworkable);
	}

	void GetRenderBounds( Vector& mins, Vector& maxs )
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef void ( __thiscall* OriginalFn )( PVOID, Vector& , Vector& );
		getvfunc<OriginalFn>( pRenderable, 20 )( pRenderable, mins, maxs );
	}
	matrix3x4& GetCoordinateFrame()
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef matrix3x4& (__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 34)(pRenderable);
	}
	int GetMaxHealth()
	{
		typedef int(__thiscall *OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 107)(this);
	}

	void SelectItem(const char *pstr, int iSubType = 0)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, const char*, int);
		return getvfunc<OriginalFn>(this, 268)(this, pstr, iSubType);
	}

	void Think()
	{
		typedef void(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 121)(this);
	}

	void PreThink()
	{
		typedef void(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 258)(this);
	}

	void PostThink()
	{
		typedef void(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 259)(this);
	}

	void SetLocalViewAngles(Vector& viewAngles)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, Vector&);
		return getvfunc<OriginalFn>(this, 299)(this, viewAngles);
	}

	void SetCurrentCommand(CUserCmd *pCmd) 
	{
		static int nOffset = gNetvars->GetOffset("DT_BasePlayer", "m_hConstraintEntity");
		*reinterpret_cast<CUserCmd**>(reinterpret_cast<DWORD>(this) + nOffset - 4) = pCmd;
	}


	int hObserverTarget()
	{
		static int m_hObserverTarget = gNetvars->GetOffset("DT_BasePlayer", "m_hObserverTarget");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_hObserverTarget);
	}

	//bool InCond(tf_cond type) const
	//{
	//	return ((_condition_bits & (1 << type)) != 0);
	//}

	CBoneAccessor& m_BoneAccessor()
	{
		//8B D9 33 C9 33 m_BoneAccessor
		using Fn = CBoneAccessor(__thiscall*)(PVOID);
		static Fn BoneAccessor = (Fn)(gSignatures.GetClientSignature("8B D9 33 C9 33") + 0x2);
		return BoneAccessor(this);
	}

	CStudioHdr* GetModelPtr()
	{
		//8B 9E ? ? ? ? 68 ? ? ? ? 8D
		using Fn = CStudioHdr*(__thiscall*)(PVOID);
		static Fn StudioHdr = (Fn)(gSignatures.GetClientSignature("8B 9E ? ? ? ? 68 ? ? ? ? 8D") + 0x2);
		return StudioHdr(this);
	}

	void CalcAbsolutePosition()
	{
		//55 8B EC 81 EC ? ? ? ? 80 3D ? ? ? ? ? 53 8B D9 0F
		using Fn = void(__thiscall*)(void*);
		static Fn AbsolutePosition = (Fn)gSignatures.GetClientSignature("55 8B EC 81 EC ? ? ? ? 80 3D ? ? ? ? ? 53 8B D9 0F");
		AbsolutePosition(this);
	}

	void UpdateButtonState(int nUserCmdButtonMask)
	{
		//55 8B EC 8B 81 ? ? ? ? 8B D0
		using Fn = void(__thiscall*)(void*, int);
		static Fn ButtonState = (Fn)gSignatures.GetClientSignature("55 8B EC 8B 81 ? ? ? ? 8B D0");
		ButtonState(this, nUserCmdButtonMask);
	}

	bool PhysicsRunThink(thinkmethods_t thinkMethod = THINK_FIRE_ALL_FUNCTIONS) 
	{
		//55 8B EC 53 8B D9 56 57 8B 83 ? ? ? ? C1
		using Fn = bool(__thiscall*)(void*, thinkmethods_t);
		static Fn RunThinkPhysics = (Fn)gSignatures.GetClientSignature("55 8B EC 53 8B D9 56 57 8B 83 ? ? ? ? C1");
		return RunThinkPhysics(this, thinkMethod);
	}
	
	//55 8B EC 8B 45 08 56 8B F1 85 C0 75 13
	int GetNextThinkTick(const char *szContext = NULL)
	{
		using Fn = int(__thiscall*)(void*, const char*);
		static Fn NextThinkTick = (Fn)gSignatures.GetClientSignature("55 8B EC 8B 45 08 56 8B F1 85 C0 75 13");
		return NextThinkTick(this, szContext);
	}

	// float nextThinkTime, const char *szContext = NULL
	void SetNextThink(float nextThinkTime, const char *szContext = NULL)
	{
		//55 8B EC 8B 81 ? ? ? ? 8B D0
		using Fn = void(__thiscall*)(void*, float, const char*);
		static Fn ButtonState = (Fn)gSignatures.GetClientSignature("55 8B EC F3 0F 10 45 ? 0F 2E 05 ? ? ? ? 53");
		ButtonState(this, nextThinkTime, szContext);
	}

	//55 8B EC 8B 45 08 85 C0 75 0C
	void SetPredictionRandomSeed(CUserCmd *cmd)
	{
		//55 8B EC 8B 81 ? ? ? ? 8B D0
		//E8 ? ? ? ? A1 ? ? ? ? 83 C4 04 89 3D
		using Fn = void(__cdecl*)(void*, CUserCmd*);
		static Fn RandomSeed = (Fn)gSignatures.GetClientSignature("55 8B EC 8B 45 08 85 C0 75 0C");
		RandomSeed(this, cmd);
	}

	bool CanPerformBackstabAgainstTarget(CBaseEntity* pTarget)
	{
		//"set_disguise_on_backstab" or "DisguiseOnKill" -> leads to PrimaryAttack
		//55 8B EC 56 8B 75 08 57 8B F9 85 F6 74 7A

		using Fn = bool(__thiscall*)(void*, CBaseEntity*);
		static Fn CanBackstab = (Fn)gSignatures.GetClientSignature("55 8B EC 51 53 56 8B 75 08 8B D9 85");
		return CanBackstab(this, pTarget);
	}


	//55 8B EC 8B 11 FF 75 08
	//Weapon_ShootPosition
	CBaseEntity* GetObserverTarget();
	CBaseCombatWeapon * GetActiveWeapon();
	CBaseCombatWeapon * GetWeaponFromSlot(int slot);

	Vector GetCollideableMins();
	Vector GetCollideableMaxs();
	CBaseEntity * GetGroundEntity();
	Vector GetBaseVelocity();
	Vector GetViewOffset();
	Vector GetEyePosition();
	Vector GetOrigin();
	Vector GetNetworkedAngles();
	Vector GetHitboxPositionCustom(int iHitbox, matrix3x4 * BoneMatrix);
	Vector GetHitboxPosition(int iHitbox);
	Vector PredictedPoint(CBaseEntity * pLocal, Vector vHitbox, Vector vVelocity);

	void UpdateGlowEffect();
	void DestroyGlowEffect();
	void SetAbsOrigin(Vector & Angle);
	void EstimateAbsVelocity(Vector & vel);
	void UpdateClientSideAnimations();

	bool & GlowEnabled();
	Vector GetEyeAngles();
	bool IsBehindAndFacingTarget(CBaseEntity * pTarget);
	CBaseEntity * GetHealingTarget();
	bool IsHoldingProjectileWeapon();
	bool CanFireWithCurrentWeapon();
	bool ComputeHitboxSurroundingBox(Vector * mins, Vector * maxs);
	void AddFlag(int flag);
	bool IsAlive();
	bool IsOnGround();

	void SetAbsAngles(Vector & Angle);

	int GetClassNum();
	int & GetCond();
	int GetHealth();
	int GetTeam();
	int GetHitboxSet();
	int & GetTickBase();

	BYTE GetLifeState();
	BYTE & GetFlags();

	Vector GetAbsVelocity();

	bool & ClientSideAnimation();
	bool Ubered();
	int GetOwner();
	bool IsPlayer();

	bool IsBuilding();

	float GetConveyorSpeed();
	float GetSimulationTime();
	float GetOldSimulationTime();
};

class CBaseCombatWeapon : public CBaseEntity
{
public:
	//IsJarateRifle = 477

	float GetProjectileSpeed()
	{
		switch (this->GetItemDefinitionIndex())
		{
		case 18:
		case 205:
		case 228:
		case 441:
		case 513:
		case 658:
		case 730:
		case 800:
		case 809:
		case 889:
		case 898:
		case 907:
		case 916:
		case 965:
		case 974:
		case 1085:
		case 1104:
		case 15006:
		case 15014:
		case 15028:
		case 15043:
		case 15052:
		case 15057:
		case 15081:
		case 15104:
		case 15105:
		case 15129:
		case 15130:
		case 15150:
			return 1100.0f;
		case 127: // Direct hit
			return 1980.0f;
		case 414: // Libery launcher
			return 1540.0f;
		case WPN_FestiveGrenadeLauncher: // Grenade launchers
		case WPN_GrenadeLauncher:
		case WPN_IronBomber:
			return 1215.0f;
		case WPN_LochNLoad: // Loch
			return 1510.0f;
		case 39: // Flareguns
		case 351:
		case 740:
			return 2000.0f;
		case 595: // Manmelter
			return 3000.0f;
		case WPN_Crossbow: case WPN_FestiveCrossbow: case WPN_RescueRanger:
			return 2400.0f;
		case WPN_Huntsman: case WPN_FestiveHuntsman: case WPN_CompoundBow:
			return this->GetArrowSpeed();
		}
		
		return 0.0f;
	}

	int GetWeaponID()
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 377)(this);
	}

	float GetArrowSpeed() //actually GetProjectileSpeed but only works on huntsman.
	{
		typedef float(__thiscall * OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 465)(this);
	}

	int GetSlot()
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 327)(this);
	}

	const char* GetName()
	{
		typedef const char*(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 329)(this);
	}

	float GetWeaponAccuracy(float speed)
	{
		typedef float(__thiscall * OriginalFn)(PVOID, float);
		return getvfunc<OriginalFn>(this, 373)(this, speed);
	}

	float GetNextPrimaryAttack()
	{
		static int m_flNextPrimaryAttack = gNetvars->GetOffset("DT_BaseCombatWeapon",  "m_flNextPrimaryAttack");
		return *(float*)((DWORD)this + m_flNextPrimaryAttack);
	}

	float GetChargeBeginTime()
	{
		static int m_flChargeBeginTime = gNetvars->GetOffset("DT_WeaponPipebombLauncher",  "m_flChargeBeginTime");
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flChargeBeginTime);
	}

	int GetItemDefinitionIndex()
	{
		static int m_iItemDefinitionIndex = gNetvars->GetOffset("DT_BaseAttributableItem", "m_iItemDefinitionIndex");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iItemDefinitionIndex);
	}

	int GetClip1()
	{
		//DT_BaseCombatWeapon m_iClip1
		static int m_iClip1 = gNetvars->GetOffset("DT_BaseCombatWeapon", "m_iClip1");
		return *(int*)((DWORD)this + m_iClip1);
	}

	float GetLastFireTime()
	{
		static int m_flLastFireTime = gNetvars->GetOffset("DT_TFWeaponBase", "m_flLastFireTime");
		return *(float*)((DWORD)this + m_flLastFireTime);
	}

	float GetUberCharge()
	{
		//evil netvar.
		static int m_flChargeLevel = gNetvars->GetOffset("DT_WeaponMedigun", "m_flChargeLevel");
		return *(float*)((DWORD)this + 0xC6C);
	}

	float GetArrowGravity() 
	{
		typedef float(__thiscall * OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 466)(this);
	}

	//these arent networked i think because they always return 0/nullptr
	float MedicGetChargeLevel(CBaseCombatWeapon** pRetMedigun = NULL)
	{
		using Fn = float(__thiscall*)(void*, CBaseCombatWeapon**);
		static Fn GetCharge = (Fn)gSignatures.GetClientSignature("55 8B EC 56 6A 05 8B");
		return GetCharge(this, pRetMedigun);
	}

	CBaseEntity* MedicGetHealTarget()
	{
		using Fn = CBaseEntity*(__thiscall*)(PVOID);
		static Fn HealTarget = (Fn)gSignatures.GetClientSignature("56 6A 05 8B F1 E8 ? ? ? ? 84 C0 74 60");
		return HealTarget(this);
	}
	DWORD * GetWpnData();
};


class IRenderView
{
public:
    virtual void vfunc00() = 0;
    virtual void vfunc01() = 0;
    virtual void vfunc02() = 0;
    virtual void vfunc03() = 0;
	virtual void SetBlend(float blend) = 0;
	virtual void vfunc05() = 0;
	virtual void SetColorModulation(float* blend) = 0;
    virtual void GetColorModulation(float* blend) = 0;
};


class IVModelInfo
{
public:
	DWORD *GetModel(int index)
	{
		typedef DWORD*(__thiscall* GetModelFn)(void*, int);
		return getvfunc<GetModelFn>(this, 1)(this, index);
	}

	int	GetModelIndex(const char* name)
	{
		typedef int(__thiscall* GetModelIndexFn)(void*, const char*);
		return getvfunc< GetModelIndexFn >(this, 2)(this, name);
	}

	const char* GetModelName(const DWORD* model)
	{
		typedef const char* (__thiscall* GetModelNameFn)(void*, const DWORD*);
		return getvfunc< GetModelNameFn >(this, 3)(this, model);
	}

	studiohdr_t* GetStudiomodel(const DWORD *mod)
	{
		typedef studiohdr_t* (__thiscall* GetStudiomodelFn)(void*, const DWORD*);
		return getvfunc< GetStudiomodelFn >(this, 28)(this, mod);
	}
};


class CObject : public CBaseEntity
{
public:
	inline int GetLevel()
	{
		static int m_iUpgradeLevel = gNetvars->GetOffset("DT_BaseObject", "m_iUpgradeLevel");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iUpgradeLevel);
	}

	inline bool IsSapped()
	{
		static int m_bHasSapper = gNetvars->GetOffset("DT_BaseObject", "m_bHasSapper");
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + m_bHasSapper);
	}

	inline bool IsBuilding()
	{
		static int m_bBuilding = gNetvars->GetOffset("DT_BaseObject", "m_bBuilding");
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + m_bBuilding);
	}

	//m_iObjectMode
	inline int GetObjectMode()
	{
		static int m_iObjectMode = gNetvars->GetOffset("DT_BaseObject", "m_iObjectMode");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iObjectMode);
	}

	inline bool IsBeingCarried()
	{
		static int m_bCarried = gNetvars->GetOffset("DT_BaseObject", "m_bCarried");
		static int m_bCarryDeploy = gNetvars->GetOffset("DT_BaseObject", "m_bCarryDeploy");
		static int m_bPlacing = gNetvars->GetOffset("DT_BaseObject", "m_bPlacing");

		if (*reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + m_bCarried)
			|| *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + m_bCarryDeploy)
			|| *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + m_bPlacing))
			return true;

		return false;
	}

	inline float GetPercentageConstructed()
	{
		static int m_flPercentageConstructed = gNetvars->GetOffset("DT_BaseObject", "m_flPercentageConstructed");

		if (IsBuilding())
			return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flPercentageConstructed);
		else
			return NULL;
	}

	inline int GetHealth()
	{
		static int m_iHealth = gNetvars->GetOffset("DT_BaseObject", "m_iHealth");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iHealth);
	}

	inline int GetMaxHealth()
	{
		static int m_iHealth = gNetvars->GetOffset("DT_BaseObject", "m_iMaxHealth");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iHealth);
	}


	inline int GetUpgradedMetal()
	{
		static int m_iUpgradeMetal = gNetvars->GetOffset("DT_BaseObject", "m_iUpgradeMetal");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iUpgradeMetal);
	}

	inline int GetState()
	{
		static int m_iState = gNetvars->GetOffset("DT_BaseObject", "m_iState");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iState);
	}

	inline int GetTeamNum()
	{
		static int m_iTeamNum = gNetvars->GetOffset("DT_BaseEntity", "m_iTeamNum");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iTeamNum);
	}
};

class CTFObjectDispenser : public CObject
{
public:
	inline int GetMetalReserve()
	{
		static int m_iAmmoMetal = gNetvars->GetOffset("DT_ObjectDispenser", "m_iAmmoMetal");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iAmmoMetal);
	}
};

class CTFObjectSentryGun : public CObject
{
public:

	inline int GetRocket()
	{
		static int m_iAmmoRockets = gNetvars->GetOffset("DT_ObjectSentrygun", "m_iAmmoRockets");

		if (GetLevel() == 3)
			return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iAmmoRockets);

		return NULL;
	}

	inline int GetAmmo()
	{
		static int m_iAmmoShells = gNetvars->GetOffset("DT_ObjectSentrygun", "m_iAmmoShells");
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + m_iAmmoShells);
	}

	inline char* GetStateString()
	{
		switch (GetState())
		{
		case 1:
		{
			return "Idle";
		}
		case 2:
		{
			return "Attacking";
		}
		case 3:
		{
			return "Upgrading";
		}
		}
		return "Unknown";
	}
};

class CTFObjectTeleporter : public CObject
{
public:

	inline float GetRechargeTime()
	{
		static int m_flRechargeTime = gNetvars->GetOffset("DT_ObjectTeleporter", "m_flRechargeTime");
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flRechargeTime);
	}

	inline float GetRechargeDuration()
	{
		static int m_flCurrentRechargeDuration = gNetvars->GetOffset("DT_ObjectTeleporter", "m_flCurrentRechargeDuration");
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flCurrentRechargeDuration);
	}

	inline float YawToExit()
	{
		static int m_flYawToExit = gNetvars->GetOffset("DT_ObjectTeleporter", "m_flYawToExit");
		return *reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(this) + m_flYawToExit);
	}

	inline char* GetStateString()
	{
		switch (GetState())
		{
		case 1:
		{
			return "Idle";
		}
		case 2:
		{
			return "Active";
		}
		case 4:
		{
			return "Teleporting";
		}
		case 6:
		{
			return "Charging";
		}
		}

		return "Unknown";
	}
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_WHATEVER
};


enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

enum class MaterialVarFlags2_t
{
	// NOTE: These are for $flags2!!!!!
	//	UNUSED											= (1 << 0),

	MATERIAL_VAR2_LIGHTING_UNLIT = 0,
	MATERIAL_VAR2_LIGHTING_VERTEX_LIT = (1 << 1),
	MATERIAL_VAR2_LIGHTING_LIGHTMAP = (1 << 2),
	MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP = (1 << 3),
	MATERIAL_VAR2_LIGHTING_MASK =
	(MATERIAL_VAR2_LIGHTING_VERTEX_LIT |
		MATERIAL_VAR2_LIGHTING_LIGHTMAP |
		MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP),

	// FIXME: Should this be a part of the above lighting enums?
	MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL = (1 << 4),
	MATERIAL_VAR2_USES_ENV_CUBEMAP = (1 << 5),
	MATERIAL_VAR2_NEEDS_TANGENT_SPACES = (1 << 6),
	MATERIAL_VAR2_NEEDS_SOFTWARE_LIGHTING = (1 << 7),
	// GR - HDR path puts lightmap alpha in separate texture...
	MATERIAL_VAR2_BLEND_WITH_LIGHTMAP_ALPHA = (1 << 8),
	MATERIAL_VAR2_NEEDS_BAKED_LIGHTING_SNAPSHOTS = (1 << 9),
	MATERIAL_VAR2_USE_FLASHLIGHT = (1 << 10),
	MATERIAL_VAR2_USE_FIXED_FUNCTION_BAKED_LIGHTING = (1 << 11),
	MATERIAL_VAR2_NEEDS_FIXED_FUNCTION_FLASHLIGHT = (1 << 12),
	MATERIAL_VAR2_USE_EDITOR = (1 << 13),
	MATERIAL_VAR2_NEEDS_POWER_OF_TWO_FRAME_BUFFER_TEXTURE = (1 << 14),
	MATERIAL_VAR2_NEEDS_FULL_FRAME_BUFFER_TEXTURE = (1 << 15),
	MATERIAL_VAR2_IS_SPRITECARD = (1 << 16),
	MATERIAL_VAR2_USES_VERTEXID = (1 << 17),
	MATERIAL_VAR2_SUPPORTS_HW_SKINNING = (1 << 18),
	MATERIAL_VAR2_SUPPORTS_FLASHLIGHT = (1 << 19),
};

enum class ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

								// Compressed normal map formats
								IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
								IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

#if defined( _X360 )
															// Depth-stencil texture formats
															IMAGE_FORMAT_X360_DST16,
															IMAGE_FORMAT_X360_DST24,
															IMAGE_FORMAT_X360_DST24F,
															// supporting these specific formats as non-tiled for procedural cpu access
															IMAGE_FORMAT_LINEAR_BGRX8888,
															IMAGE_FORMAT_LINEAR_RGBA8888,
															IMAGE_FORMAT_LINEAR_ABGR8888,
															IMAGE_FORMAT_LINEAR_ARGB8888,
															IMAGE_FORMAT_LINEAR_BGRA8888,
															IMAGE_FORMAT_LINEAR_RGB888,
															IMAGE_FORMAT_LINEAR_BGR888,
															IMAGE_FORMAT_LINEAR_BGRX5551,
															IMAGE_FORMAT_LINEAR_I8,
															IMAGE_FORMAT_LINEAR_RGBA16161616,

															IMAGE_FORMAT_LE_BGRX8888,
															IMAGE_FORMAT_LE_BGRA8888,
#endif

															IMAGE_FORMAT_DXT1_RUNTIME,
															IMAGE_FORMAT_DXT5_RUNTIME,

															NUM_IMAGE_FORMATS
};
//-----------------------------------------------------------------------------
// Preview image return values
//-----------------------------------------------------------------------------
enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};
//EB 71 8B 84
//struct Ray_t;
class KeyValues;
class IMaterialVar;
//class ITexture;
//class IMaterialProxy;
class Vector;
typedef unsigned short MaterialHandle_t;
// undefined types
class MorphFormat_t;
class VertexFormat_t;
class MaterialPropertyTypes_t;
class IMaterial
{
public:
	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char *	GetName() const = 0;
	virtual const char *	GetTextureGroupName() const = 0;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	virtual PreviewImageRetVal_t GetPreviewImageProperties(int *width, int *height,
		ImageFormat *imageFormat, bool* isTranslucent) const = 0;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual PreviewImageRetVal_t GetPreviewImage(unsigned char *data,
		int width, int height,
		ImageFormat imageFormat) const = 0;
	// 
	virtual int				GetMappingWidth() = 0;
	virtual int				GetMappingHeight() = 0;

	virtual int				GetNumAnimationFrames() = 0;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool			InMaterialPage(void) = 0;
	virtual	void			GetMaterialOffset(float *pOffset) = 0;
	virtual void			GetMaterialScale(float *pScale) = 0;
	virtual IMaterial		*GetMaterialPage(void) = 0;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual IMaterialVar *	FindVar(const char *varName, bool *found, bool complain = true) = 0;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount(void) = 0;
	virtual void			DecrementReferenceCount(void) = 0;

	inline void AddRef() { IncrementReferenceCount(); }
	inline void Release() { DecrementReferenceCount(); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID(void) const = 0;

	virtual void			GetLowResColorSample(float s, float t, float *color) const = 0;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots() = 0;

	// Are we translucent?
	virtual bool			IsTranslucent() = 0;

	// Are we alphatested?
	virtual bool			IsAlphaTested() = 0;

	// Are we vertex lit?
	virtual bool			IsVertexLit() = 0;

	// Gets the vertex format
	virtual VertexFormat_t	GetVertexFormat() const = 0;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy(void) const = 0;

	virtual bool			UsesEnvCubemap(void) = 0;

	virtual bool			NeedsTangentSpace(void) = 0;

	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning(void) = 0;

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate(float alpha) = 0;
	virtual void			ColorModulate(float r, float g, float b) = 0;

	// Material Var flags...
	virtual void			SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
	virtual bool			GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;

	// Gets material reflectivity
	virtual void			GetReflectivity(Vector& reflect) = 0;

	// Gets material property flags
	virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type) = 0;

	// Is the material visible from both sides?
	virtual bool			IsTwoSided() = 0;

	// Sets the shader associated with the material
	virtual void			SetShader(const char *pShaderName) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses(void) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes(void) = 0;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh() = 0;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha(void) = 0;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting(void) = 0;

	// Gets at the shader parameters
	virtual int				ShaderParamCount() const = 0;
	virtual IMaterialVar	**GetShaderParams(void) = 0;

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial() const = 0;

	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation() = 0;
	virtual void			GetColorModulation(float *r, float *g, float *b) = 0;

	// Gets the morph format
	virtual MorphFormat_t	GetMorphFormat() const = 0;

	// fast find that stores the index of the found var in the string table in local cache
	virtual IMaterialVar *	FindVarFast(char const *pVarName, unsigned int *pToken) = 0;

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams(KeyValues *pKeyValues) = 0;
	virtual const char *	GetShaderName() const = 0;

	virtual void			DeleteIfUnreferenced() = 0;

	virtual bool			IsSpriteCard() = 0;

	virtual void			CallBindProxy(void *proxyData) = 0;

	virtual IMaterial		*CheckProxyReplacement(void *proxyData) = 0;

	virtual void			RefreshPreservingMaterialVars() = 0;

	virtual bool			WasReloadedFromWhitelist() = 0;

	virtual bool			IsPrecached() const = 0;
};

class IMaterialSystem
{
public:
	IMaterial* FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL)
	{
		typedef IMaterial *(__thiscall *OriginalFn)(PVOID, const char *, const char *, bool, const char *);
		return getvfunc<OriginalFn>(this, 73)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}

	IMaterial* CreateMaterial(char const* pMaterialName, KeyValues* kv)
	{
		typedef IMaterial *(__thiscall *OriginalFn)(PVOID, const char*, KeyValues*);
		return getvfunc<OriginalFn>(this, 72)(this, pMaterialName, kv);
	}

	IMaterial* GetMaterial(MaterialHandle_t h)
	{
		typedef IMaterial *(__thiscall *OriginalFn)(PVOID, MaterialHandle_t);
		return getvfunc<OriginalFn>(this, 78)(this, h);
	}

	MaterialHandle_t FirstMaterial()
	{
		typedef MaterialHandle_t(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 75)(this);
	}

	MaterialHandle_t InvalidMaterial()
	{
		typedef MaterialHandle_t(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 77)(this);
	}

	MaterialHandle_t NextMaterial(MaterialHandle_t h)
	{
		typedef MaterialHandle_t(__thiscall *OriginalFn)(PVOID, MaterialHandle_t);
		return getvfunc<OriginalFn>(this, 76)(this, h);
	}
};

class IVModelRender
{
public:
	void ForcedMaterialOverride(IMaterial *mat, OverrideType_t type = OverrideType_t::OVERRIDE_NORMAL)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, IMaterial *, OverrideType_t);
		return getvfunc<OriginalFn>(this, 1)(this, mat, type);
	}
};


struct ModelRenderInfo_t
{
	Vector origin;
	Vector angles;
	void *pRenderable;
	const void *pModel;
	const matrix3x4 *pModelToWorld;
	const matrix3x4 *pLightingOffset;
	const Vector *pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	unsigned short instance;

	ModelRenderInfo_t()
	{
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};

struct studiohdr_t;
struct studiohwdata_t;
typedef void* StudioDecalHandle_t;
class IClientRenderable;
struct DrawModelState_t
{
	studiohdr_t*			m_pStudioHdr;
	studiohwdata_t*			m_pStudioHWData;
	IClientRenderable*		m_pRenderable;
	const matrix3x4			*m_pModelToWorld;
	StudioDecalHandle_t*	m_decals;
	int						m_drawFlags;
	int						m_lod;
};


class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface(const char *pInterfaceName) = 0;

	// Init, shutdown
	virtual int Init() = 0;
	virtual void Shutdown(char* reason) = 0;

};

enum AnalogCode_t //needed but not rly
{

};

class InputEvent_t;

enum ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP,		// A fake button which is 'pressed' and 'released' when the wheel is moved up 
	MOUSE_WHEEL_DOWN,	// A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1,
};

//We dont have it O.o
class Color;
class IInputSystem : public IAppSystem
{
public:
	// Attach, detach input system from a particular window
	// This window should be the root window for the application
	// Only 1 window should be attached at any given time.
	virtual void AttachToWindow(void* hWnd) = 0;
	virtual void DetachFromWindow() = 0;

	// Enables/disables input. PollInputState will not update current 
	// button/analog states when it is called if the system is disabled.
	virtual void EnableInput(bool bEnable) = 0;

	// Enables/disables the windows message pump. PollInputState will not
	// Peek/Dispatch messages if this is disabled
	virtual void EnableMessagePump(bool bEnable) = 0;

	// Polls the current input state
	virtual void PollInputState() = 0;

	// Gets the time of the last polling in ms
	virtual int GetPollTick() const = 0;

	// Is a button down? "Buttons" are binary-state input devices (mouse buttons, keyboard keys)
	virtual bool IsButtonDown(ButtonCode_t code) const = 0;

	// Returns the tick at which the button was pressed and released
	virtual int GetButtonPressedTick(ButtonCode_t code) const = 0;
	virtual int GetButtonReleasedTick(ButtonCode_t code) const = 0;

	// Gets the value of an analog input device this frame
	// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogValue(AnalogCode_t code) const = 0;

	// Gets the change in a particular analog input device this frame
	// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogDelta(AnalogCode_t code) const = 0;

	// Returns the input events since the last poll
	virtual int GetEventCount() const = 0;
	virtual const InputEvent_t* GetEventData() const = 0;

	// Posts a user-defined event into the event queue; this is expected
	// to be called in overridden wndprocs connected to the root panel.
	virtual void PostUserEvent(const InputEvent_t &event) = 0;

	// Returns the number of joysticks
	virtual int GetJoystickCount() const = 0;

	// Enable/disable joystick, it has perf costs
	virtual void EnableJoystickInput(int nJoystick, bool bEnable) = 0;

	// Enable/disable diagonal joystick POV (simultaneous POV buttons down)
	virtual void EnableJoystickDiagonalPOV(int nJoystick, bool bEnable) = 0;

	// Sample the joystick and append events to the input queue
	virtual void SampleDevices(void) = 0;

	// FIXME: Currently force-feedback is only supported on the Xbox 360
	virtual void SetRumble(float fLeftMotor, float fRightMotor, int userId = -1) = 0;
	virtual void StopRumble(void) = 0;

	// Resets the input state
	virtual void ResetInputState() = 0;

	// Sets a player as the primary user - all other controllers will be ignored.
	virtual void SetPrimaryUserId(int userId) = 0;

	// Convert back + forth between ButtonCode/AnalogCode + strings
	virtual const char *ButtonCodeToString(ButtonCode_t code) const = 0;
	virtual const char *AnalogCodeToString(AnalogCode_t code) const = 0;
	virtual ButtonCode_t StringToButtonCode(const char *pString) const = 0;
	virtual AnalogCode_t StringToAnalogCode(const char *pString) const = 0;

	// Sleeps until input happens. Pass a negative number to sleep infinitely
	virtual void SleepUntilInput(int nMaxSleepTimeMS = -1) = 0;

	// Convert back + forth between virtual codes + button codes
	// FIXME: This is a temporary piece of code
	virtual ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey) const = 0;
	virtual int ButtonCodeToVirtualKey(ButtonCode_t code) const = 0;
	virtual ButtonCode_t ScanCodeToButtonCode(int lParam) const = 0;

	// How many times have we called PollInputState?
	virtual int GetPollCount() const = 0;

	// Sets the cursor position
	virtual void SetCursorPosition(int x, int y) = 0;

	// NVNT get address to haptics interface
	virtual void *GetHapticsInterfaceAddress() const = 0;

	virtual void SetNovintPure(bool bPure) = 0;

	// read and clear accumulated raw input values
	virtual bool GetRawMouseAccumulators(int& accumX, int& accumY) = 0;

	// tell the input system that we're not a game, we're console text mode.
	// this is used for dedicated servers to not initialize joystick system.
	// this needs to be called before CInputSystem::Init (e.g. in PreInit of
	// some system) if you want ot prevent the joystick system from ever
	// being initialized.
	virtual void SetConsoleTextMode(bool bConsoleTextMode) = 0;
};
class ICvar : public IAppSystem
{
public:
	// Allocate a unique DLL identifier
	virtual int AllocateDLLIdentifier() = 0;

	// Register, unregister commands
	virtual void			RegisterConCommand(ConCommandBase *pCommandBase) = 0;
	virtual void			UnregisterConCommand(ConCommandBase *pCommandBase) = 0;
	virtual void			UnregisterConCommands(int id) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char*		GetCommandLineValue(const char *pVariableName) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase *FindCommandBase(const char *name) = 0;
	virtual const ConCommandBase *FindCommandBase(const char *name) const = 0;
	virtual ConVar			*FindVar(const char *var_name) = 0;
	virtual const ConVar	*FindVar(const char *var_name) const = 0;
	virtual ConCommand		*FindCommand(const char *name) = 0;
	virtual const ConCommand *FindCommand(const char *name) const = 0;

	// Get first ConCommandBase to allow iteration
	virtual ConCommandBase	*GetCommands(void) = 0;
	virtual const ConCommandBase *GetCommands(void) const = 0;

	// Install a global change callback (to be called when any convar changes)
	virtual void			InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			CallGlobalChangeCallbacks(ConVar *var, const char *pOldString, float flOldValue) = 0;

	// Install a console printer
	virtual void			InstallConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			RemoveConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			ConsoleColorPrintf(const Color& clr, const char *pFormat, ...) const = 0;
	virtual void			ConsolePrintf(const char *pFormat, ...) const = 0;
	virtual void			ConsoleDPrintf(const char *pFormat, ...) const = 0;

	// Reverts cvars which contain a specific flag
	virtual void			RevertFlaggedConVars(int nFlag) = 0;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICVar, so we can't use the standard connect pattern
	virtual void			InstallCVarQuery(void *pQuery) = 0;

#if defined( _X360 )
	virtual void			PublishToVXConsole() = 0;
#endif
	virtual bool			IsMaterialThreadSetAllowed() const = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, const char *pValue) = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, int nValue) = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, float flValue) = 0;
	virtual bool			HasQueuedMaterialThreadConVarSets() const = 0;
	virtual int				ProcessQueuedMaterialThreadConVarSets() = 0;

protected:	class ICVarIteratorInternal;
public:
	/// Iteration over all cvars.
	/// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
	/// usage:
	/// { ICVar::Iterator iter(g_pCVar);
	///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	///   {
	///       ConCommandBase *cmd = iter.Get();
	///   }
	/// }
	/// The Iterator class actually wraps the internal factory methods
	/// so you don't need to worry about new/delete -- scope takes care
	//  of it.
	/// We need an iterator like this because we can't simply return a
	/// pointer to the internal data type that contains the cvars --
	/// it's a custom, protected class with unusual semantics and is
	/// prone to change.
	class Iterator
	{
	public:
		inline Iterator(ICvar *icvar);
		inline ~Iterator(void);
		inline void		SetFirst(void);
		inline void		Next(void);
		inline bool		IsValid(void);
		inline ConCommandBase *Get(void);
	private:
		ICVarIteratorInternal *m_pIter;
	};

protected:
	// internals for  ICVarIterator
	class ICVarIteratorInternal
	{
	public:
		// warning: delete called on 'ICvar::ICVarIteratorInternal' that is abstract but has non-virtual destructor [-Wdelete-non-virtual-dtor]
		virtual ~ICVarIteratorInternal()
		{
		}
		virtual void		SetFirst(void) = 0;
		virtual void		Next(void) = 0;
		virtual	bool		IsValid(void) = 0;
		virtual ConCommandBase *Get(void) = 0;
	};

	virtual ICVarIteratorInternal	*FactoryInternalIterator(void) = 0;
	friend class Iterator;
};

typedef struct netadr_s
{
public:
	netadr_s()
	{
		SetIP(0);
		SetPort(0);
		//SetType(NA_IP);
	}
	netadr_s(const char *pch)
	{
		SetFromString(pch);
	}
	void Clear(); // invalids Address

				  //void SetType(netadrtype_t type);
	void SetPort(unsigned short port);
	bool SetFromSockadr(const struct sockaddr *s);
	void SetIP(unsigned int unIP); // Sets IP.  unIP is in host order (little-endian)
	void SetIPAndPort(unsigned int unIP, unsigned short usPort)
	{
		SetIP(unIP);
		SetPort(usPort);
	}
	void SetFromString(const char *pch, bool bUseDNS = false); // if bUseDNS is true then do a DNS lookup if needed

	bool CompareAdr(const netadr_s &a, bool onlyBase = false) const;
	bool CompareClassBAdr(const netadr_s &a) const;
	bool CompareClassCAdr(const netadr_s &a) const;

	//netadrtype_t GetType() const;
	unsigned short GetPort() const;
	const char *ToString(bool onlyBase = false) const; // returns xxx.xxx.xxx.xxx:ppppp
	void ToSockadr(struct sockaddr *s) const;
	unsigned int GetIP() const;

	bool IsLocalhost() const;   // true, if this is the localhost IP
	bool IsLoopback() const;	// true if engine loopback buffers are used
	bool IsReservedAdr() const; // true, if this is a private LAN IP
	bool IsValid() const;		// ip & port != 0
	void SetFromSocket(int hSocket);
	// These function names are decorated because the Xbox360 defines macros for ntohl and htonl
	unsigned long addr_ntohl() const;
	unsigned long addr_htonl() const;
	bool operator==(const netadr_s &netadr) const
	{
		return (CompareAdr(netadr));
	}
	bool operator<(const netadr_s &netadr) const;

public: // members are public to avoid to much changes
		//netadrtype_t type;
	unsigned char ip[4];
	unsigned short port;
} netadr_t;


#define MAX_FLOWS 2 // in & out
#define MAX_STREAMS 2
#define MAX_OSPATH 260
#define SUBCHANNEL_FREE 0	// subchannel is free to use

class INetMessage;
class INetChannelHandler;
class INetChannelInfo
{
public:

	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char  *GetName(void) const = 0;	// get channel name
	virtual const char  *GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetTotalPackets(int flow) const = 0;
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int *received, int *total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float *pflFrameTime, float *pflFrameTimeStdDeviation, float *pflFrameStartTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};
class INetChannel : public INetChannelInfo
{
public:
	virtual ~INetChannel(void)
	{
	};

	virtual void SetDataRate(float rate) = 0;
	virtual bool RegisterMessage(INetMessage *msg) = 0;
	virtual bool StartStreaming(unsigned int challengeNr) = 0;
	virtual void ResetStreaming(void) = 0;
	virtual void SetTimeout(float seconds) = 0;
	//virtual void SetDemoRecorder(IDemoRecorder *recorder) = 0;
	virtual void SetChallengeNr(unsigned int chnr) = 0;

	virtual void Reset(void) = 0;
	virtual void Clear(void) = 0;
	virtual void Shutdown(const char *reason) = 0;

	virtual void ProcessPlayback(void) = 0;
	virtual bool ProcessStream(void) = 0;
	virtual void ProcessPacket(struct netpacket_s *packet, bool bHasHeader) = 0;

	virtual bool SendNetMsg(INetMessage &msg, bool bForceReliable = false, bool bVoice = false) = 0;
#ifdef POSIX
	FORCEINLINE bool SendNetMsg(INetMessage const &msg, bool bForceReliable = false, bool bVoice = false)
	{
		return SendNetMsg(*((INetMessage *)&msg), bForceReliable, bVoice);
	}
#endif
	//virtual bool SendData(bf_write &msg, bool bReliable = true) = 0;
	virtual bool SendFile(const char *filename, unsigned int transferID) = 0;
	virtual void DenyFile(const char *filename, unsigned int transferID) = 0;
	virtual void RequestFile_OLD(const char *filename, unsigned int transferID) = 0; // get rid of this function when we version the
	virtual void SetChoked(void) = 0;
	//virtual int SendDatagram(bf_write *data) = 0;
	virtual bool Transmit(bool onlyReliable = false) = 0;

	virtual const netadr_t &GetRemoteAddress(void) const = 0;
	virtual INetChannelHandler *GetMsgHandler(void) const = 0;
	virtual int GetDropNumber(void) const = 0;
	virtual int GetSocket(void) const = 0;
	virtual unsigned int GetChallengeNr(void) const = 0;
	virtual void GetSequenceData(int &nOutSequenceNr, int &nInSequenceNr, int &nOutSequenceNrAck) = 0;
	virtual void SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;

	virtual void UpdateMessageStats(int msggroup, int bits) = 0;
	virtual bool CanPacket(void) const = 0;
	virtual bool IsOverflowed(void) const = 0;
	virtual bool IsTimedOut(void) const = 0;
	virtual bool HasPendingReliableData(void) = 0;

	virtual void SetFileTransmissionMode(bool bBackgroundMode) = 0;
	virtual void SetCompressionMode(bool bUseCompression) = 0;
	virtual unsigned int RequestFile(const char *filename) = 0;
	virtual float GetTimeSinceLastReceived(void) const = 0; // get time since last received packet in seconds

	virtual void SetMaxBufferSize(bool bReliable, int nBYTEs, bool bVoice = false) = 0;

	virtual bool IsNull() const = 0;
	virtual int GetNumBitsWritten(bool bReliable) = 0;
	virtual void SetInterpolationAmount(float flInterpolationAmount) = 0;
	virtual void SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) = 0;

	// Max # of payload BYTEs before we must split/fragment the packet
	virtual void SetMaxRoutablePayloadSize(int nSplitSize) = 0;
	virtual int GetMaxRoutablePayloadSize() = 0;

	virtual int GetProtocolVersion() = 0;
	};
class INetMessage
{
public:
	virtual ~INetMessage()
	{
	};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void SetNetChannel(INetChannel *netchan) = 0; // netchannel this message is from/for
	virtual void SetReliable(bool state) = 0;			  // set to true if it's a reliable message

	virtual bool Process(void) = 0; // calles the recently set handler to process this message

	virtual bool ReadFromBuffer(uintptr_t &buffer) = 0; // returns true if parsing was OK
	virtual bool WriteToBuffer(uintptr_t &buffer) = 0; // returns true if writing was OK

	virtual bool IsReliable(void) const = 0; // true, if message needs reliable handling

	virtual int GetType(void) const = 0;		 // returns module specific header tag eg svc_serverinfo
	virtual int GetGroup(void) const = 0;		 // returns net message group of this message
	virtual const char *GetName(void) const = 0; // returns network message name, eg "svc_serverinfo"
	virtual INetChannel *GetNetChannel(void) const = 0;
	virtual const char *ToString(void) const = 0; // returns a human readable string about message content
};
class INetChannelHandler
{
public:
	virtual ~INetChannelHandler(void)
	{
	};

	virtual void ConnectionStart(INetChannel *chan) = 0; // called first time network channel is established

	virtual void ConnectionClosing(const char *reason) = 0; // network channel is being closed by remote site

	virtual void ConnectionCrashed(const char *reason) = 0; // network error occured

	virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged) = 0; // called each time a new packet arrived

	virtual void PacketEnd(void) = 0; // all messages has been parsed

	virtual void FileRequested(const char *fileName, unsigned int transferID) = 0; // other side request a file for download

	virtual void FileReceived(const char *fileName, unsigned int transferID) = 0; // we received a file

	virtual void FileDenied(const char *fileName, unsigned int transferID) = 0; // a file request was denied by other side

	virtual void FileSent(const char *fileName, unsigned int transferID) = 0; // we sent a file
};
typedef void *FileHandle_t;
class CNetChannel : public INetChannel
{

public: // netchan structurs
	typedef struct dataFragments_s
	{
		FileHandle_t file;				// open file handle
		char filename[MAX_OSPATH];		// filename
		char *buffer;					// if NULL it's a file
		unsigned int BYTEs;				// size in BYTEs
		unsigned int bits;				// size in bits
		unsigned int transferID;		// only for files
		bool isCompressed;				// true if data is bzip compressed
		unsigned int nUncompressedSize; // full size in BYTEs
		bool asTCP;						// send as TCP stream
		int numFragments;				// number of total fragments
		int ackedFragments;				// number of fragments send & acknowledged
		int pendingFragments;			// number of fragments send, but not acknowledged yet
	} dataFragments_t;

	struct subChannel_s
	{
		int startFraggment[MAX_STREAMS];
		int numFragments[MAX_STREAMS];
		int sendSeqNr;
		int state; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 = dirty
		int index; // index in m_SubChannels[]

		void Free()
		{
			state = SUBCHANNEL_FREE;
			sendSeqNr = -1;
			for (int i = 0; i < MAX_STREAMS; i++)
			{
				numFragments[i] = 0;
				startFraggment[i] = -1;
			}
		}
	};

	// Client's now store the command they sent to the server and the entire results set of
	//  that command.
	typedef struct netframe_s
	{
		// Data received from server
		float time;		   // net_time received/send
		int size;		   // total size in BYTEs
		float latency;	 // raw ping for this packet, not cleaned. set when acknowledged otherwise -1.
		float avg_latency; // averaged ping for this packet
		bool valid;		   // false if dropped, lost, flushed
		int choked;		   // number of previously chocked packets
		int dropped;
		float m_flInterpolationAmount;
		unsigned short msggroups[INetChannelInfo::TOTAL]; // received BYTEs for each message group
	} netframe_t;

	typedef struct
	{
		float nextcompute;		  // Time when we should recompute k/sec data
		float avgBYTEspersec;	 // average BYTEs/sec
		float avgpacketspersec;   // average packets/sec
		float avgloss;			  // average packet loss [0..1]
		float avgchoke;			  // average packet choke [0..1]
		float avglatency;		  // average ping, not cleaned
		float latency;			  // current ping, more accurate also more jittering
		int totalpackets;		  // total processed packets
		int totalBYTEs;			  // total processed BYTEs
		int currentindex;		  // current frame index
		netframe_t frames[64];	// frame history
		netframe_t *currentframe; // current frame
	} netflow_t;

public:
	bool m_bProcessingMessages;
	bool m_bShouldDelete;

	// last send outgoing sequence number
	int m_nOutSequenceNr;
	// last received incoming sequnec number
	int m_nInSequenceNr;
	// last received acknowledge outgoing sequnce number
	int m_nOutSequenceNrAck;

	// state of outgoing reliable data (0/1) flip flop used for loss detection
	int m_nOutReliableState;
	// state of incoming reliable data
	int m_nInReliableState;

	int m_nChokedPackets; //number of choked packets

						  // Reliable data buffer, send which each packet (or put in waiting list)
						  //bf_write m_StreamReliable;
						  //CUtlMemory<BYTE> m_ReliableDataBuffer;

						  // unreliable message buffer, cleared which each packet
						  //bf_write m_StreamUnreliable;
						  //CUtlMemory<BYTE> m_UnreliableDataBuffer;

						  //bf_write m_StreamVoice;
						  //CUtlMemory<BYTE> m_VoiceDataBuffer;

						  // don't use any vars below this (only in net_ws.cpp)

	int m_Socket;		// NS_SERVER or NS_CLIENT index, depending on channel.
	int m_StreamSocket; // TCP socket handle

	unsigned int m_MaxReliablePayloadSize; // max size of reliable payload in a single packet

										   // Address this channel is talking to.
	netadr_t remote_address;

	// For timeouts.  Time last message was received.
	float last_received;
	// Time when channel was connected.
	double connect_time;

	// Bandwidth choke
	// BYTEs per second
	int m_Rate;
	// If realtime > cleartime, free to send next packet
	double m_fClearTime;

	//CUtlVector<dataFragments_t *> m_WaitingList[MAX_STREAMS]; // waiting list for reliable data and file transfer
	dataFragments_t m_ReceiveList[MAX_STREAMS];				  // receive buffers for streams
	subChannel_s m_SubChannels[8];

	unsigned int m_FileRequestCounter; // increasing counter with each file request
	bool m_bFileBackgroundTranmission; // if true, only send 1 fragment per packet
	bool m_bUseCompression;			   // if true, larger reliable data will be bzip compressed

									   // TCP stream state maschine:
	bool m_StreamActive;		   // true if TCP is active
	int m_SteamType;			   // STREAM_CMD_*
	int m_StreamSeqNr;			   // each blob send of TCP as an increasing ID
	int m_StreamLength;			   // total length of current stream blob
	int m_StreamReceived;		   // length of already received BYTEs
								   //char m_SteamFile[MAX_OSPATH];  // if receiving file, this is it's name
								   //CUtlMemory<BYTE> m_StreamData; // Here goes the stream data (if not file). Only allocated if we're going to use it.

								   // packet history
	netflow_t m_DataFlow[MAX_FLOWS];
	int m_MsgStats[INetChannelInfo::TOTAL]; // total BYTEs for each message group

	int m_PacketDrop; // packets lost before getting last update (was global net_drop)

	char m_Name[32]; // channel name

	unsigned int m_ChallengeNr; // unique, random challenge number

	float m_Timeout; // in seconds

	INetChannelHandler *m_MessageHandler;	// who registers and processes messages
											//CUtlVector<INetMessage *> m_NetMessages; // list of registered message
											//IDemoRecorder *m_DemoRecorder;			 // if != NULL points to a recording/playback demo object
	int m_nQueuedPackets;

	float m_flInterpolationAmount;
	float m_flRemoteFrameTime;
	float m_flRemoteFrameTimeStdDeviation;
	int m_nMaxRoutablePayloadSize;

	int m_nSplitPacketSequence;
};


class EngineClient
{
public:
	void GetScreenSize( int& width, int& height )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int& , int& );
		return getvfunc<OriginalFn>( this, 5 )( this, width, height );
	}
	bool GetPlayerInfo( int ent_num, player_info_t *pinfo )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID, int, player_info_t * );
		return getvfunc<OriginalFn>(this, 8)(this, ent_num, pinfo );
	}
	int GetPlayerForUserID(int ent_num)
	{
		typedef int(__thiscall* OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(this, 9)(this, ent_num);
	}
	/*bool Con_IsVisible( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 11 )( this );
	}*/
	int GetLocalPlayer( void )
	{
		typedef int ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 12 )( this );
	}
	float Time( void )
	{
		typedef float ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 14 )( this );
	}
	void GetViewAngles( Vector& va )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, Vector& va );
		return getvfunc<OriginalFn>( this, 19 )( this, va );
	}
	void SetViewAngles( Vector& va )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, Vector& va );
		return getvfunc<OriginalFn>( this, 20 )( this, va );
	}
	int GetMaxClients( void )
	{
		typedef int ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 21 )( this );
	}
	bool IsInGame( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 26 )( this );
	}
	bool IsConnected( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 27 )( this );
	}
	bool IsDrawingLoadingImage( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 28 )( this );
	}
	const matrix3x4& WorldToScreenMatrix( void )
	{
		typedef const matrix3x4& ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>(this, 36)(this);
	}
	bool IsPaused(void)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 84)(this);
	}
	bool IsTakingScreenshot( void )
	{
		typedef bool ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 85 )( this );
	}
	//reusing the same pasted code. i wanna kms.
	CNetChannel* GetNetChannelInfo( void )
	{
		typedef CNetChannel* ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 72 )( this );
	}
	void ClientCmd_Unrestricted( const char* chCommandString )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, const char * );
		return getvfunc<OriginalFn>( this, 106 )( this, chCommandString );
	}
	void ServerCmdKeyValues(PVOID kv)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, PVOID);
		getvfunc<OriginalFn>(this, 127)(this, kv);
	}
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

class CViewSetup
{
public:
	uint32_t x; //0x0000 
	uint32_t m_nUnscaledX; //0x0004 
	uint32_t y; //0x0008 
	uint32_t m_nUnscaledY; //0x000C 
	uint32_t width; //0x0010 
	uint32_t m_nUnscaledWidth; //0x0014 
	uint32_t height; //0x0018 
	uint32_t m_eStereoEye; //0x001C 
	uint32_t m_nUnscaledHeight; //0x0020 
	bool m_bOrtho; //0x0024 
	float m_OrthoLeft; //0x0028 
	float m_OrthoTop; //0x002C 
	float m_OrthoRight; //0x0030 
	float m_OrthoBottom; //0x0034 
	float m_fov; //0x0038 
	float m_viewmodel_fov; //0x003C 
	Vector origin; //0x0040 
	Vector angles; //0x004C 
	float zNear; //0x0058 
	float zFar; //0x005C 
	float viewmodel_zNear; //0x0060 
	float viewmodel_zFar; //0x0064 
	bool m_bRenderToSubrectOfLargerScreen; //0x0068 
	float m_flAspectRatio; //0x006C 
	bool m_bOffCenter; //0x0070 
	float m_flOffCenterTop; //0x0074 
	float m_flOffCenterBottom; //0x0078 
	float m_flOffCenterLeft; //0x007C 
	float m_flOffCenterRight; //0x0080 
	bool m_bDoBloomAndToneMapping; //0x0084 
	bool m_bCacheFullSceneState; //0x0088 
	bool m_bViewToProjectionOverride; //0x008C 
	matrix3x4 m_ViewToProjection; //0x0090 
};

class IGameEvent
{
public:
	virtual ~IGameEvent() { };
	virtual const char* GetName() const = 0;

	virtual bool IsReliable() const = 0;
	virtual bool IsLocal() const = 0;
	virtual bool IsEmpty(const char* key_name = 0) = 0;

	virtual bool		GetBool(const char* key_name = 0, bool default_valye = false) = 0;
	virtual int			GetInt(const char* key_name = 0, int default_valye = 0) = 0;
	virtual float		GetFloat(const char* key_name = 0, float default_valye = 0.0f) = 0;
	virtual const char* GetString(const char* key_name = 0, const char* default_valye = "") = 0;

	virtual void SetBool(const char* key_name, bool			new_value) = 0;
	virtual void SetInt(const char* key_name, int			new_value) = 0;
	virtual void SetFloat(const char* key_name, float		new_value) = 0;
	virtual void SetString(const char* key_namee, const char*  new_value) = 0;
};

class IGameEventListener
{
public:
	virtual ~IGameEventListener(void) {};
	virtual void FireGameEvent(IGameEvent *event) = 0;
};

class IGameEventManager
{
public:
	bool AddListener(IGameEventListener* listener, const char* name, bool server_sided)
	{
		using original_fn = bool(__thiscall*)(void*, IGameEventListener*, const char*, bool);
		return getvfunc<original_fn>(this, 3)(this, listener, name, server_sided);
	}
};


enum TFObjectType
{
	TFObject_CartDispenser = 0,
	TFObject_Dispenser = 0,
	TFObject_Teleporter = 1,
	TFObject_Sentry = 2,
	TFObject_Sapper = 3
};


enum TFObjectMode
{
	TFObjectMode_None = 0,
	TFObjectMode_Entrance = 0,
	TFObjectMode_Exit = 1
};

enum tf_hitboxes
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};


enum tf_cond
{
	TFCond_Slowed = (1 << 0), //Toggled when a player is slowed down. 
	TFCond_Zoomed = (1 << 1), //Toggled when a player is zoomed. 
	TFCond_Disguising = (1 << 2), //Toggled when a Spy is disguising.  
	TFCond_Disguised = (1 << 3), //Toggled when a Spy is disguised. 
	TFCond_Cloaked = (1 << 4), //Toggled when a Spy is invisible. 
	TFCond_Ubercharged = (1 << 5), //Toggled when a player is ÜberCharged. 
	TFCond_TeleportedGlow = (1 << 6), //Toggled when someone leaves a teleporter and has glow beneath their feet. 
	TFCond_Taunting = (1 << 7), //Toggled when a player is taunting. 
	TFCond_UberchargeFading = (1 << 8), //Toggled when the ÜberCharge is fading. 
	TFCond_CloakFlicker = (1 << 9), //Toggled when a Spy is visible during cloak. 
	TFCond_Teleporting = (1 << 10), //Only activates for a brief second when the player is being teleported; not very useful. 
	TFCond_Kritzkrieged = (1 << 11), //Toggled when a player is being crit buffed by the KritzKrieg. 
	TFCond_TmpDamageBonus = (1 << 12), //Unknown what this is for. Name taken from the AlliedModders SDK. 
	TFCond_DeadRingered = (1 << 13), //Toggled when a player is taking reduced damage from the Deadringer. 
	TFCond_Bonked = (1 << 14), //Toggled when a player is under the effects of The Bonk! Atomic Punch. 
	TFCond_Stunned = (1 << 15), //Toggled when a player's speed is reduced from airblast or a Sandman ball. 
	TFCond_Buffed = (1 << 16), //Toggled when a player is within range of an activated Buff Banner. 
	TFCond_Charging = (1 << 17), //Toggled when a Demoman charges with the shield. 
	TFCond_DemoBuff = (1 << 18), //Toggled when a Demoman has heads from the Eyelander. 
	TFCond_CritCola = (1 << 19), //Toggled when the player is under the effect of The Crit-a-Cola. 
	TFCond_InHealRadius = (1 << 20), //Unused condition, name taken from AlliedModders SDK. 
	TFCond_Healing = (1 << 21), //Toggled when someone is being healed by a medic or a dispenser. 
	TFCond_OnFire = (1 << 22), //Toggled when a player is on fire. 
	TFCond_Overhealed = (1 << 23), //Toggled when a player has >100% health. 
	TFCond_Jarated = (1 << 24), //Toggled when a player is hit with a Sniper's Jarate. 
	TFCond_Bleeding = (1 << 25), //Toggled when a player is taking bleeding damage. 
	TFCond_DefenseBuffed = (1 << 26), //Toggled when a player is within range of an activated Battalion's Backup. 
	TFCond_Milked = (1 << 27), //Player was hit with a jar of Mad Milk. 
	TFCond_MegaHeal = (1 << 28), //Player is under the effect of Quick-Fix charge. 
	TFCond_RegenBuffed = (1 << 29), //Toggled when a player is within a Concheror's range. 
	TFCond_MarkedForDeath = (1 << 30), //Player is marked for death by a Fan O'War hit. Effects are similar to TFCond_Jarated. 
	TFCond_NoHealingDamageBuff = (1 << 31), //Unknown what this is used for.

	TFCondEx_SpeedBuffAlly = (1 << 0), //Toggled when a player gets hit with the disciplinary action. 
	TFCondEx_HalloweenCritCandy = (1 << 1), //Only for Scream Fortress event maps that drop crit candy. 
	TFCondEx_CritCanteen = (1 << 2), //Player is getting a crit boost from a MVM canteen.
	TFCondEx_CritDemoCharge = (1 << 3), //From demo's shield
	TFCondEx_CritHype = (1 << 4), //Soda Popper crits. 
	TFCondEx_CritOnFirstBlood = (1 << 5), //Arena first blood crit buff. 
	TFCondEx_CritOnWin = (1 << 6), //End of round crits. 
	TFCondEx_CritOnFlagCapture = (1 << 7), //CTF intelligence capture crits. 
	TFCondEx_CritOnKill = (1 << 8), //Unknown what this is for. 
	TFCondEx_RestrictToMelee = (1 << 9), //Unknown what this is for. 
	TFCondEx_DefenseBuffNoCritBlock = (1 << 10), //MvM Buff.
	TFCondEx_Reprogrammed = (1 << 11), //MvM Bot has been reprogrammed.
	TFCondEx_PyroCrits = (1 << 12), //Player is getting crits from the Mmmph charge. 
	TFCondEx_PyroHeal = (1 << 13), //Player is being healed from the Mmmph charge. 
	TFCondEx_FocusBuff = (1 << 14), //Player is getting a focus buff.
	TFCondEx_DisguisedRemoved = (1 << 15), //Disguised remove from a bot.
	TFCondEx_MarkedForDeathSilent = (1 << 16), //Player is under the effects of the Escape Plan/Equalizer or GRU.
	TFCondEx_DisguisedAsDispenser = (1 << 17), //Bot is disguised as dispenser.
	TFCondEx_Sapped = (1 << 18), //MvM bot is being sapped.
	TFCondEx_UberchargedHidden = (1 << 19), //MvM Related
	TFCondEx_UberchargedCanteen = (1 << 20), //Player is receiving ÜberCharge from a canteen.
	TFCondEx_HalloweenBombHead = (1 << 21), //Player has a bomb on their head from Merasmus.
	TFCondEx_HalloweenThriller = (1 << 22), //Players are forced to dance from Merasmus.
	TFCondEx_BulletCharge = (1 << 26), //Player is receiving 75% reduced damage from bullets.
	TFCondEx_ExplosiveCharge = (1 << 27), //Player is receiving 75% reduced damage from explosives.
	TFCondEx_FireCharge = (1 << 28), //Player is receiving 75% reduced damage from fire.
	TFCondEx_BulletResistance = (1 << 29), //Player is receiving 10% reduced damage from bullets.
	TFCondEx_ExplosiveResistance = (1 << 30), //Player is receiving 10% reduced damage from explosives.
	TFCondEx_FireResistance = (1 << 31), //Player is receiving 10% reduced damage from fire.

	TFCondEx2_Stealthed = (1 << 0),
	TFCondEx2_MedigunDebuff = (1 << 1),
	TFCondEx2_StealthedUserBuffFade = (1 << 2),
	TFCondEx2_BulletImmune = (1 << 3),
	TFCondEx2_BlastImmune = (1 << 4),
	TFCondEx2_FireImmune = (1 << 5),
	TFCondEx2_PreventDeath = (1 << 6),
	TFCondEx2_MVMBotRadiowave = (1 << 7),
	TFCondEx2_HalloweenSpeedBoost = (1 << 8), //Wheel has granted player speed boost.
	TFCondEx2_HalloweenQuickHeal = (1 << 9), //Wheel has granted player quick heal.
	TFCondEx2_HalloweenGiant = (1 << 10), //Wheel has granted player giant mode.
	TFCondEx2_HalloweenTiny = (1 << 11), //Wheel has granted player tiny mode.
	TFCondEx2_HalloweenInHell = (1 << 12), //Wheel has granted player in hell mode.
	TFCondEx2_HalloweenGhostMode = (1 << 13), //Wheel has granted player ghost mode.
	TFCondEx2_Parachute = (1 << 16), //Player has deployed the BASE Jumper.
	TFCondEx2_BlastJumping = (1 << 17), //Player has sticky or rocket jumped.

	TFCond_MiniCrits = (TFCond_Buffed | TFCond_CritCola),
	TFCond_IgnoreStates = (TFCond_Ubercharged | TFCond_Bonked),
	TFCondEx_IgnoreStates = (TFCondEx_PyroHeal)
};

enum tf_classes
{
	TF2_Scout = 1,
	TF2_Soldier = 3,
	TF2_Pyro = 7,
	TF2_Demoman = 4,
	TF2_Heavy = 6,
	TF2_Engineer = 9,
	TF2_Medic = 5,
	TF2_Sniper = 2,
	TF2_Spy = 8,
};

class IPanel
{
public:
	const char *GetName(unsigned int vguiPanel)
	{
		typedef const char* ( __thiscall* OriginalFn )( PVOID, unsigned int );
		return getvfunc<OriginalFn>( this, 36 )( this, vguiPanel );
	}
	void SetMouseInputEnabled(unsigned int panel, bool state)
	{
		getvfunc<void(__thiscall *)(void*, int, bool)>(this, 32)(this, panel, state);
	}
};

class ISurface
{
public:
	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 11)(this, r, g, b, a);
	}
	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 12)(this, x0, y0, x1, y1);
	}
	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 14)(this, x0, y0, x1, y1);
	}
	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long);
		getvfunc<OriginalFn>(this, 17)(this, font);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 19)(this, r, g, b, a);
	}
	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int);
		getvfunc<OriginalFn>(this, 20)(this, x, y);
	}
	void DrawPrintText(const wchar_t *text, int textLen)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, const wchar_t *, int, int);
		return getvfunc<OriginalFn>(this, 22)(this, text, textLen, 0);
	}
	unsigned long CreateFont()
	{
		typedef unsigned int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 66)(this);
	}
	void SetFontGlyphSet(unsigned long &font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int);
		getvfunc<OriginalFn>(this, 67)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}
	void GetTextSize(unsigned long font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const wchar_t *, int&, int&);
		getvfunc<OriginalFn>(this, 75)(this, font, text, wide, tall);
	}
	void DrawLine(int x, int y, int x1, int y1)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, int, int, int, int);
		return getvfunc<OriginalFn>(this, 15)(this, x, y, x1, y1);
	}

};

class CEntList
{
public:
	CBaseEntity* GetClientEntity( int entnum )
	{
		typedef CBaseEntity* ( __thiscall* OriginalFn )( PVOID, int );
		return getvfunc<OriginalFn>( this, 3 )( this, entnum );
	}
	CBaseEntity* GetClientEntityFromHandle( int hEnt )
	{
		typedef CBaseEntity* ( __thiscall* OriginalFn )( PVOID, int );
		return getvfunc<OriginalFn>( this, 4 )( this, hEnt );
	}
	int GetHighestEntityIndex(void)
	{
		typedef int ( __thiscall* OriginalFn )( PVOID );
		return getvfunc<OriginalFn>( this, 6 )( this );
	}
};

// lower bits are stronger, and will eat weaker brushes completely
#define CONTENTS_EMPTY 0 // No contents

#define CONTENTS_SOLID 0x1  // an eye is never valid in a solid
#define CONTENTS_WINDOW 0x2 // translucent, but not watery (glass)
#define CONTENTS_AUX 0x4
#define CONTENTS_GRATE 0x8 // alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define CONTENTS_SLIME 0x10
#define CONTENTS_WATER 0x20
#define CONTENTS_BLOCKLOS 0x40 // block AI line of sight
#define CONTENTS_OPAQUE 0x80   // things that cannot be seen through (may be non-solid though)
#define LAST_VISIBLE_CONTENTS 0x80

#define ALL_VISIBLE_CONTENTS ( LAST_VISIBLE_CONTENTS | ( LAST_VISIBLE_CONTENTS - 1 ) )

#define CONTENTS_TESTFOGVOLUME 0x100
#define CONTENTS_UNUSED 0x200

// unused
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
#define CONTENTS_UNUSED6 0x400

#define CONTENTS_TEAM1 0x800  // per team contents used to differentiate collisions
#define CONTENTS_TEAM2 0x1000 // between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE 0x4000

// remaining contents are non-visible, and don't eat brushes
#define CONTENTS_AREAPORTAL 0x8000

#define CONTENTS_PLAYERCLIP 0x10000
#define CONTENTS_MONSTERCLIP 0x20000

// currents can be added to any other contents, and may be mixed
#define CONTENTS_CURRENT_0 0x40000
#define CONTENTS_CURRENT_90 0x80000
#define CONTENTS_CURRENT_180 0x100000
#define CONTENTS_CURRENT_270 0x200000
#define CONTENTS_CURRENT_UP 0x400000
#define CONTENTS_CURRENT_DOWN 0x800000

#define CONTENTS_ORIGIN 0x1000000 // removed before bsping an entity

#define CONTENTS_MONSTER 0x2000000 // should never be on a brush, only in game
#define CONTENTS_DEBRIS 0x4000000
#define CONTENTS_DETAIL 0x8000000		// brushes to be added after vis leafs
#define CONTENTS_TRANSLUCENT 0x10000000 // auto set if any surface has trans
#define CONTENTS_LADDER 0x20000000
#define CONTENTS_HITBOX 0x40000000 // use accurate hitboxes on trace

#define MASK_ALL ( 0xFFFFFFFF )
// everything that is normally solid
#define MASK_SOLID ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE )
// everything that blocks player movement
#define MASK_PLAYERSOLID ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE )
// blocks npc movement
#define MASK_NPCSOLID ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE )
// water physics in these contents
#define MASK_WATER ( CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME )
// everything that blocks lighting
#define MASK_OPAQUE ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_OPAQUE )
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS ( MASK_OPAQUE | CONTENTS_MONSTER )
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS )
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS ( MASK_BLOCKLOS | CONTENTS_MONSTER )
// everything that blocks line of sight for players
#define MASK_VISIBLE ( MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE )
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS ( MASK_OPAQUE_AND_NPCS | CONTENTS_IGNORE_NODRAW_OPAQUE )
// bullets see these as solid
#define MASK_SHOT ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX )
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE )
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER )
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE )
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE )
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY ( CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE )
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC ( CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE )
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL ( CONTENTS_WATER | CONTENTS_SLIME )

// UNDONE: This is untested, any moving water
#define MASK_CURRENT ( CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN )

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define MASK_DEADSOLID ( CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE )

class __declspec(align(16))VectorAligned : public Vector
{
public:
	inline VectorAligned(void)
	{

	};

	inline VectorAligned(float X, float Y, float Z)
	{
		Init(X, Y, Z);
	}

	explicit VectorAligned(const Vector &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
	}

	VectorAligned &operator=(const Vector &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}

	float w;
};

struct Ray_t
{
	VectorAligned   m_Start;
	VectorAligned   m_Delta;
	VectorAligned   m_StartOffset;
	VectorAligned   m_Extents;

	bool    m_IsRay;
	bool    m_IsSwept;

	void Init(Vector& start, Vector& end)
	{
		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents.Init();
		m_IsRay = true;

		m_StartOffset.Init();
		m_Start = start;
	}

	void Init(Vector& start, Vector& end, Vector& mins, Vector& maxs)
	{
		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSqr() < 1e-6);

		m_StartOffset = mins + maxs;
		m_StartOffset *= 0.5f;
		m_Start = start - m_StartOffset;
		m_StartOffset *= -1.0f;
	}
};

struct cplane_t
{
	Vector normal;
	float	dist;
	BYTE	type;
	BYTE	signbits;
	BYTE	pad[2];
};

struct csurface_t
{
	const char		*name;
	short			surfaceProps;
	unsigned short	flags;
};

enum SurfaceFlags_t
{
	DISPSURF_FLAG_SURFACE = (1 << 0),
	DISPSURF_FLAG_WALKABLE = (1 << 1),
	DISPSURF_FLAG_BUILDABLE = (1 << 2),
	DISPSURF_FLAG_SURFPROP1 = (1 << 3),
	DISPSURF_FLAG_SURFPROP2 = (1 << 4),
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(void *pEntity, int contentsMask) = 0;
	virtual TraceType_t	GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
public:
	virtual bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		CBaseEntity *pEntity = (CBaseEntity *)pEntityHandle;

		switch (pEntity->GetClientClass()->iClassID)
		{
		case 55: // Portal Window
		case 64: // Spawn Door visualizers
		case 117: // Sniper Dots
		case 225: // Medigun Shield
			return false;
			break;
		}

		return !(pEntityHandle == pSkip);
	}

	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CBaseTrace
{
public:
	bool IsDispSurface(void) { return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void) { return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void) { return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceProp1(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceProp2(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:
	Vector			startpos;
	Vector			endpos;
	cplane_t		plane;

	float			fraction;

	int				contents;
	unsigned short	dispFlags;

	bool			allsolid;
	bool			startsolid;

	CBaseTrace() {}

private:
	CBaseTrace(const CBaseTrace& vOther);
};

class CGameTrace : public CBaseTrace
{
public:
	bool DidHitWorld() const;

	bool DidHitNonWorldEntity() const;

	int GetEntityIndex() const;

	bool DidHit() const
	{
		return fraction < 1 || allsolid || startsolid;
	}

public:
	float			fractionleftsolid;
	csurface_t		surface;

	int				hitgroup;

	short			physicsbone;

	CBaseEntity*	m_pEnt;
	int				hitbox;

	CGameTrace() {}
	CGameTrace(const CGameTrace& vOther);
};

class IEngineTrace
{
public:	  //We really only need this I guess...
	void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace)//5
	{
		typedef void(__thiscall* TraceRayFn)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
		return getvfunc<TraceRayFn>(this, 4)(this, ray, fMask, pTraceFilter, pTrace);
	}
};

class CMoveData
{
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;

	int m_nPlayerHandle; // edict index on server, client entity handle on client

	int    m_nImpulseCommand; // Impulse command issued.
	Vector m_vecViewAngles; // Command view angles (local space)
	Vector m_vecAbsViewAngles; // Command view angles (world space)
	int    m_nButtons; // Attack buttons.
	int    m_nOldButtons; // From host_client->oldbuttons;
	float  m_flForwardMove;
	float  m_flSideMove;
	float  m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	Vector m_vecVelocity; // edict::velocity		// Current movement direction.
	Vector m_vecAngles; // edict::angles
	Vector m_vecOldAngles;

	// Output only
	float  m_outStepHeight; // how much you climbed this move
	Vector m_outWishVel; // This is where you tried
	Vector m_outJumpVel; // This is your jump velocity

	// Movement constraints	(radius 0 means no constraint)
	Vector m_vecConstraintCenter;
	float  m_flConstraintRadius;
	float  m_flConstraintWidth;
	float  m_flConstraintSpeedFactor;

	void          SetAbsOrigin(const Vector &vec);
	const Vector &GetAbsOrigin() const;

	// private:
	Vector m_vecAbsOrigin; // edict::origin
};

class IMoveHelper
{
public:
	//what do i need this for?
};

class IGameMovement
{
public:
	virtual ~IGameMovement(void)
	{
	}

	// Process the current movement command
	virtual void ProcessMovement(CBaseEntity *pPlayer, CMoveData *pMove) = 0;
	virtual void StartTrackPredictionErrors(CBaseEntity *pPlayer) = 0;
	virtual void FinishTrackPredictionErrors(CBaseEntity *pPlayer) = 0;
	virtual void DiffPrint(char const *fmt, ...) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual Vector GetPlayerMins(bool ducked) const = 0;
	virtual Vector GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector GetPlayerViewOffset(bool ducked) const = 0;
};

class CPrediction
{
public:
	void SetLocalViewAngles(Vector& ang)
	{
		return getvfunc<void(__thiscall *)(void *, Vector&)>(this, 16)(this, ang);
	}
	void SetupMove(CBaseEntity *player, CUserCmd *ucmd, IMoveHelper *helper, CMoveData *move)
	{
		return getvfunc<void(__thiscall *)(void *, CBaseEntity *, CUserCmd *, IMoveHelper *, CMoveData *)>(this, 18)(this, player, ucmd, helper, move);
	}
	void FinishMove(CBaseEntity *player, CUserCmd *ucmd, CMoveData *move)
	{
		return getvfunc<void(__thiscall *)(void *, CBaseEntity *, CUserCmd *, CMoveData *)>(this, 19)(this, player, ucmd, move);
	}
};

class CDemoFile;
struct netpacket_t;
class IDemoPlayer
{
public:
	virtual ~IDemoPlayer() {};

	virtual CDemoFile *GetDemoFile() = 0;
	virtual int		GetPlaybackStartTick(void) = 0;
	virtual int		GetPlaybackTick(void) = 0;
	virtual int		GetTotalTicks(void) = 0;

	virtual bool	StartPlayback(const char *filename, bool bAsTimeDemo) = 0;

	virtual bool	IsPlayingBack(void) = 0; // true if demo loaded and playing back
	virtual bool	IsPlaybackPaused(void) = 0; // true if playback paused
	virtual bool	IsPlayingTimeDemo(void) = 0; // true if playing back in timedemo mode
	virtual bool	IsSkipping(void) = 0; // true, if demo player skiiping trough packets
	virtual bool	CanSkipBackwards(void) = 0; // true if demoplayer can skip backwards

	virtual void	SetPlaybackTimeScale(float timescale) = 0; // sets playback timescale
	virtual float	GetPlaybackTimeScale(void) = 0; // get playback timescale

	virtual void	PausePlayback(float seconds) = 0; // pause playback n seconds, -1 = forever
	virtual void	SkipToTick(int tick, bool bRelative, bool bPause) = 0; // goto a specific tick, 0 = start, -1 = end
	virtual void	SetEndTick(int tick) = 0;	// set end tick
	virtual void	ResumePlayback(void) = 0; // resume playback
	virtual void	StopPlayback(void) = 0;	// stop playback, close file
	virtual void	InterpolateViewpoint() = 0; // override viewpoint
	virtual netpacket_t *ReadPacket(void) = 0; // read packet from demo file

	virtual void	ResetDemoInterpolation() = 0;
	virtual int		GetProtocolVersion() = 0;

	virtual bool	ShouldLoopDemos() = 0;		// if we're in "startdemos" - should we loop?
	virtual void	OnLastDemoInLoopPlayed() = 0;	// Last demo of "startdemos" just completed

	virtual bool	IsLoading(void) = 0; // true if demo is currently loading
};

struct GlowObjectDefinition_t
{
	int m_hEntity;
	Vector m_vGlowColor;
	float m_flGlowAlpha;
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	int m_nSplitScreenSlot;
	int m_nNextFreeSlot;
};

class CGlowObjectManager
{
public:
	CUtlVector<GlowObjectDefinition_t> m_GlowObjectDefinitions;
	int m_nFirstFreeSlot;
};

class CGameRules
{
public:
	bool IsPVEModeActive()
	{
		typedef bool(__thiscall* IsPVEModeFn)(void*);
		static DWORD dwInstructionPtr = gSignatures.GetClientSignature("E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B 03 8B CB 68");
		static DWORD dwMask = dwInstructionPtr + 1;
		static DWORD dwFunction = ((*(PDWORD)(dwMask)) + dwMask + 4);
		static IsPVEModeFn IsPVE = (IsPVEModeFn)(dwFunction);
		return IsPVE(this);
	}

	bool IsMatchTypeCasual()
	{
		typedef bool(__thiscall* IsMatchTypeCasual)(void*);
		static DWORD dwInstructionPtr = gSignatures.GetClientSignature("E8 ? ? ? ? F6 D8 1A C0 22");
		static DWORD dwMask = dwInstructionPtr + 1;
		static DWORD dwFunction = ((*(PDWORD)(dwMask)) + dwMask + 4);
		static IsMatchTypeCasual IsCasual = (IsMatchTypeCasual)(dwFunction);
		return IsCasual(this);
	}
};

enum playercontrols
{
	IN_ATTACK = (1 << 0),
	IN_JUMP	= (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_ATTACK2 = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_ALT1 = (1 << 14),
	IN_ALT2 = (1 << 15),
	IN_SCORE = (1 << 16),	// Used by client.dll for when scoreboard is held down
	IN_SPEED = (1 << 17),	// Player is holding the speed key
	IN_WALK = (1 << 18),	// Player holding walk key
	IN_ZOOM	= (1 << 19),	// Zoom key for HUD zoom
	IN_WEAPON1 = (1 << 20),	// weapon defines these bits
	IN_WEAPON2 = (1 << 21),	// weapon defines these bits
	IN_BULLRUSH = (1 << 22),
};

enum source_lifestates
{
	LIFE_ALIVE,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

class ClientModeShared
{
public:
	bool IsChatPanelOutOfFocus(void)
	{
		typedef PVOID(__thiscall* OriginalFn)(PVOID);
		PVOID CHudChat = getvfunc<OriginalFn>(this, 19)(this);
		if (CHudChat)
		{
			return *(PFLOAT)((DWORD)CHudChat + 0xFC) == 0;
		}
		return false;
	}
};


class CInterfaces
{
public:
	CEntList* EntList;
	EngineClient* Engine;
	IPanel* Panels;
	ISurface* Surface;
	ClientModeShared* ClientMode;
	CHLClient* Client;
	CDebugOverlay* DebugOverlay;
	CGlobals* Globals;
	IMoveHelper* MoveHelper;
	IMaterialSystem* MaterialSystem;
	IVModelRender* ModelRender;
	IVModelInfo* ModelInfo;
	IGameEventManager* GameEvent;
	ICvar* Cvar;
	IEngineTrace* EngineTrace;
	IRenderView* RenderView;
	CGlowObjectManager* GlowObjectManager;
	IDemoPlayer* DemoPlayer;
	CGameRules* GameRules;
	IGameMovement* GameMovement;
	CPrediction* Prediction;
};

using OverrideViewFn = int(__stdcall*)(CViewSetup*);
extern OverrideViewFn oView;

using CreateMoveFn = bool(__thiscall*)(PVOID, float, CUserCmd*);
extern CreateMoveFn bReturn;

using ShouldDrawFogFn = bool(__thiscall*)(PVOID);
extern ShouldDrawFogFn oFog;

using IsHltvFn = bool(__thiscall*)(PVOID);
extern IsHltvFn oIsHLTV;

using PanelsFn = void(__thiscall*)(PVOID, unsigned int, bool, bool);
extern PanelsFn oPanel;

using FrameStageFn = void(__thiscall*)(PVOID, ClientFrameStage_t);
extern FrameStageFn oFrameStage;

using RunCommandFn = void(__thiscall *)(void*, CBaseEntity*, CUserCmd *, IMoveHelper *);
extern RunCommandFn oRunCommand;

using DrawModelExecuteFn = void(__thiscall *)(IVModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*);
extern DrawModelExecuteFn oDrawModel;

using LevelInitPreEntity = void(__stdcall *)(const char*);
extern LevelInitPreEntity oLevelInit;

using LevelInitPostEntity = void(__stdcall *)( );
extern LevelInitPostEntity oLevelPost;

using IsPlayingBackFn = bool(__thiscall*)(PVOID);
extern IsPlayingBackFn oIsPlayingBack;

using IsPlayingTimeDemoFn = bool(__thiscall*)(PVOID);
extern IsPlayingTimeDemoFn oIsPlayingTimeDemo;

using OuterAbsVelocityFn = void(__thiscall*)(PVOID, Vector&);
extern OuterAbsVelocityFn oGetOuterAbsVelocity;

using EstimateAbsVelocityFn = void(__thiscall*)(CBaseEntity*, Vector&);
extern EstimateAbsVelocityFn oEstimateAbsVelocity;

using UpdatePlayerListFn = void(__thiscall*)(PVOID);
extern UpdatePlayerListFn oUpdatePlayerList;

using AddFlagFn = void(__thiscall*)(PVOID, int);
extern AddFlagFn oAddFlag;

using TeleportedFn = bool(__thiscall*)(PVOID);
extern TeleportedFn oTeleported;

using ItemPostFrameFn = void(__thiscall*)(PVOID);
extern ItemPostFrameFn oItemPostFrame;

using UpdateClientSideAnimationsFn = void(__thiscall*)(CBaseEntity*);
extern UpdateClientSideAnimationsFn oUpdateClientSideAnimations;

extern CInterfaces gInts;
extern CPlayerVariables gPlayerVars;
extern COffsets gOffsets;
extern CFriends gFriends;
extern CConvars gConvars;