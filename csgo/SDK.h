#pragma once
#include <windows.h>
#include <math.h>
#include <xstring>
#include "Vector.h"
#include "getvfunc.h"
#include "CSignature.h"
#include "WeaponList.h"
#include "ConVar.h"
#include "CGlobalVars.h"
#include "Log.h"
#include "minhook/minhook.h"
#include "NetVar.h"
#include <string>
#include <ctime>
#include <thread>
#include "MD5.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "Checksum.h"

//using namespace std;

#pragma optimize("gsy",on)
#pragma warning( disable : 4244 ) //Possible loss of data

typedef void* ( __cdecl* CreateInterface_t )( const char*, int* );
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
typedef float matrix3x4[3][4];
#include "studio.h"


#define streql(x,y) strcmp( (x) , (y) ) == 0
#define	IMPORT(DLL, FUNC) GetProcAddress(GetModuleHandle(DLL), FUNC)
#define MAX_PLAYER_NAME_LENGTH		32
#define ENABLE_INTERPOLATION 1
#define DISABLE_INTERPOLATION 0
#define DOD_SNIPER_ZOOM_CHANGE_TIME 1
#define WIN32_LEAN_AND_MEAN

#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

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
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define WEHRMACHT 3
#define AXIS 2

#define BONE_USED_BY_ANYTHING		0x0007FF00
typedef struct player_info_s
{
	int64_t __pad0;
	union {
		int64_t xuid;
		struct {
			int xuidlow;
			int xuidhigh;
		};
	};
	char name[128];
	int userid;
	char guid[33];
	unsigned int friendsid;
	char friendsname[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
} player_info_t;

enum
{
	HITBOX_HEAD = 0,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_R_THIGH,
	HITBOX_L_THIGH,
	HITBOX_R_CALF,
	HITBOX_L_CALF,
	HITBOX_R_FOOT,
	HITBOX_L_FOOT,
	HITBOX_R_HAND,
	HITBOX_L_HAND,
	HITBOX_R_UPPER_ARM,
	HITBOX_R_FOREARM,
	HITBOX_L_UPPER_ARM,
	HITBOX_L_FOREARM,
	HITBOX_MAX
};

enum  
{
	HITGROUP_GENERIC = 0,
	HITGROUP_HEAD = 1,
	HITGROUP_CHEST = 2,
	HITGROUP_STOMACH = 3,
	HITGROUP_LEFTARM = 4,
	HITGROUP_RIGHTARM = 5,
	HITGROUP_LEFTLEG = 6,
	HITGROUP_RIGHTLEG = 7,
	HITGROUP_NECK = 8,
	HITGROUP_GEAR = 10
};

class CSWeaponData
{
public:
	char pad_0000[4]; //0x0000
	char* ConsoleName; //0x0004
	char pad_0008[12]; //0x0008
	int iMaxClip1; //0x0014
	char pad_0018[12]; //0x0018
	int iMaxClip2; //0x0024
	char pad_0028[4]; //0x0028
	char* szWorldModel; //0x002C
	char* szViewModel; //0x0030
	char* szDropedModel; //0x0034
	char pad_0038[4]; //0x0038
	char* N00000984; //0x003C
	char pad_0040[56]; //0x0040
	char* szEmptySound; //0x0078
	char pad_007C[4]; //0x007C
	char* szBulletType; //0x0080
	char pad_0084[4]; //0x0084
	char* szHudName; //0x0088
	char* szWeaponName; //0x008C
	char pad_0090[60]; //0x0090
	int WeaponType; //0x00CC
	int iWeaponPrice; //0x00D0
	int iKillAward; //0x00D4
	char* szAnimationPrefex; //0x00D8
	float flCycleTime; //0x00DC
	float flCycleTimeAlt; //0x00E0
	float flTimeToIdle; //0x00E4
	float flIdleInterval; //0x00E8
	bool bFullAuto; //0x00EC
	char pad_00ED[3]; //0x00ED
	int iDamage; //0x00F0
	float flArmorRatio; //0x00F4
	int iBullets; //0x00F8
	float flPenetration; //0x00FC
	float flFlinchVelocityModifierLarge; //0x0100
	float flFlinchVelocityModifierSmall; //0x0104
	float flRange; //0x0108
	float flRangeModifier; //0x010C
	char pad_0110[28]; //0x0110
	int iCrosshairMinDistance; //0x012C
	float flMaxPlayerSpeed; //0x0130
	float flMaxPlayerSpeedAlt; //0x0134
	char pad_0138[4]; //0x0138
	float flSpread; //0x013C
	float flSpreadAlt; //0x0140
	float flInaccuracyCrouch; //0x0144
	float flInaccuracyCrouchAlt; //0x0148
	float flInaccuracyStand; //0x014C
	float flInaccuracyStandAlt; //0x0150
	float flInaccuracyJumpIntial; //0x0154
	float flInaccaurcyJumpApex;
	float flInaccuracyJump; //0x0158
	float flInaccuracyJumpAlt; //0x015C
	float flInaccuracyLand; //0x0160
	float flInaccuracyLandAlt; //0x0164
	float flInaccuracyLadder; //0x0168
	float flInaccuracyLadderAlt; //0x016C
	float flInaccuracyFire; //0x0170
	float flInaccuracyFireAlt; //0x0174
	float flInaccuracyMove; //0x0178
	float flInaccuracyMoveAlt; //0x017C
	float flInaccuracyReload; //0x0180
	int iRecoilSeed; //0x0184
	float flRecoilAngle; //0x0188
	float flRecoilAngleAlt; //0x018C
	float flRecoilVariance; //0x0190
	float flRecoilAngleVarianceAlt; //0x0194
	float flRecoilMagnitude; //0x0198
	float flRecoilMagnitudeAlt; //0x019C
	float flRecoilMagnatiudeVeriance; //0x01A0
	float flRecoilMagnatiudeVerianceAlt; //0x01A4
	float flRecoveryTimeCrouch; //0x01A8
	float flRecoveryTimeStand; //0x01AC
	float flRecoveryTimeCrouchFinal; //0x01B0
	float flRecoveryTimeStandFinal; //0x01B4
	int iRecoveryTransititionStartBullet; //0x01B8
	int iRecoveryTransititionEndBullet; //0x01BC
	bool bUnzoomAfterShot; //0x01C0
	char pad_01C1[31]; //0x01C1
	char* szWeaponClass; //0x01E0
	char pad_01E4[56]; //0x01E4
	float flInaccuracyPitchShift; //0x021C
	float flInaccuracySoundThreshold; //0x0220
	float flBotAudibleRange; //0x0224
	char pad_0228[12]; //0x0228
	bool bHasBurstMode; //0x0234
};

enum  
{
	CONTENTS_EMPTY = 0,
	CONTENTS_SOLID = 0x1,
	CONTENTS_WINDOW = 0x2,
	CONTENTS_AUX = 0x4,
	CONTENTS_GRATE = 0x8,
	CONTENTS_SLIME = 0x10,
	CONTENTS_WATER = 0x20,
	CONTENTS_BLOCKLOS = 0x40,
	CONTENTS_OPAQUE = 0x80,
	CONTENTS_TESTFOGVOLUME = 0x100,
	CONTENTS_UNUSED = 0x200,
	CONTENTS_BLOCKLIGHT = 0x400,
	CONTENTS_TEAM1 = 0x800,
	CONTENTS_TEAM2 = 0x1000,
	CONTENTS_IGNORE_NODRAW_OPAQUE = 0x2000,
	CONTENTS_MOVEABLE = 0x4000,
	CONTENTS_AREAPORTAL = 0x8000,
	CONTENTS_PLAYERCLIP = 0x10000,
	CONTENTS_MONSTERCLIP = 0x20000,
	CONTENTS_CURRENT_0 = 0x40000,
	CONTENTS_CURRENT_90 = 0x80000,
	CONTENTS_CURRENT_180 = 0x100000,
	CONTENTS_CURRENT_270 = 0x200000,
	CONTENTS_CURRENT_UP = 0x400000,
	CONTENTS_CURRENT_DOWN = 0x800000,
	CONTENTS_ORIGIN = 0x1000000,
	CONTENTS_MONSTER = 0x2000000,
	CONTENTS_DEBRIS = 0x4000000,
	CONTENTS_DETAIL = 0x8000000,
	CONTENTS_TRANSLUCENT = 0x10000000,
	CONTENTS_LADDER = 0x20000000,
	CONTENTS_HITBOX = 0x40000000,
};

enum
{
	MASK_ALL = 0xFFFFFFFF,
	MASK_SOLID = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_PLAYERSOLID = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_NPCSOLID = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_NPCFLUID = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE,
	MASK_WATER = CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME,
	MASK_OPAQUE = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_OPAQUE,
	MASK_OPAQUE_AND_NPCS = MASK_OPAQUE | CONTENTS_MONSTER,
	MASK_BLOCKLOS = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS,
	MASK_BLOCKLOS_AND_NPCS = MASK_BLOCKLOS | CONTENTS_MONSTER,
	MASK_VISIBLE = MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE,
	MASK_VISIBLE_AND_NPCS = MASK_OPAQUE_AND_NPCS | CONTENTS_IGNORE_NODRAW_OPAQUE,
	MASK_SHOT = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE | CONTENTS_HITBOX,
	MASK_SHOT_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_DEBRIS,
	MASK_SHOT_HULL = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE,
	MASK_SHOT_PORTAL = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER,
	MASK_SOLID_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE,
	MASK_PLAYERSOLID_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE,
	MASK_NPCSOLID_BRUSHONLY = CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE,
	MASK_NPCWORLDSTATIC = CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE,
	MASK_NPCWORLDSTATIC_FLUID = CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP,
	MASK_SPLITAREPORTAL = CONTENTS_WATER | CONTENTS_SLIME,
	MASK_CURRENT = CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP | CONTENTS_CURRENT_DOWN,
	MASK_DEADSOLID = CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE,
};
typedef enum
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
	DPT_String,
	DPT_Array,	// An array of the base types (can't be of datatables).
	DPT_DataTable,
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
	DPT_Quaternion,
#endif

#ifdef SUPPORTS_INT64
	DPT_Int64,
#endif

	DPT_NUMSendPropTypes

} SendPropType;

struct RecvProp;
class DVariant
{
public:
	union
	{
		float m_Float;
		long m_Int;
		char* m_pString;
		void* m_pData;
		float m_Vector[3];
	};
};

class CRecvProxyData
{
public:
	const RecvProp* m_pRecvProp;
	char _pad[4];//csgo ( for l4d keep it commented out :) )
	DVariant m_Value;
	int m_iElement;
	int m_ObjectID;
};


struct RecvTable
{
	RecvProp* m_pProps;
	int m_nProps;
	void* m_pDecoder;
	char* m_pNetTableName;
	bool m_bInitialized;
	bool m_bInMainList;
};

struct RecvProp
{
	char* m_pVarName;
	int m_RecvType;
	int m_Flags;
	int m_StringBufferSize;
	bool m_bInsideArray;
	const void* m_pExtraData;
	RecvProp* m_pArrayProp;
	void* m_ArrayLengthProxy;
	void* m_ProxyFn;
	void* m_DataTableProxyFn;
	RecvTable* m_pDataTable;
	int m_Offset;
	int m_ElementStride;
	int m_nElements;
	const char* m_pParentArrayPropName;
};

class IClientUnknown;
class ClientClass;
class bf_read;
class IClientNetworkable
{
public:
	virtual IClientUnknown*  GetIClientUnknown() = 0;
	virtual void             Release() = 0;
	virtual ClientClass*     GetClientClass() = 0;
	virtual void             NotifyShouldTransmit(int state) = 0;
	virtual void             OnPreDataChanged(int updateType) = 0;
	virtual void             OnDataChanged(int updateType) = 0;
	virtual void             PreDataUpdate(int updateType) = 0;
	virtual void             PostDataUpdate(int updateType) = 0;
	virtual void             __unkn(void) = 0;
	virtual bool             IsDormant(void) = 0;
	virtual int				 GetIndex(void) const = 0;
	virtual int              EntIndex(void) const = 0;
	virtual void             ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void*            GetDataTableBasePtr() = 0;
	virtual void             SetDestroyedOnRecreateEntities(void) = 0;
};

typedef IClientNetworkable*	(*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable* (*CreateEventFn)();

class ClientClass
{
public:
	CreateClientClassFn		m_pCreateFn;
	CreateEventFn m_pCreateEventFn;
	char			*chName;
	RecvTable		*Table;
	ClientClass		*pNextClass;
	int				iClassID;
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

class CGlobals
{
public:
	const float realtime;
	const int framecount;
	const float absoluteFrameTime;
	const byte pad[4];
	float curtime;
	float frametime;
	const int maxClients;
	const int tickCount;
	const float interval_per_tick;
};

class CDebugOverlay
{
public:
	bool ScreenPosition(const Vector& vIn, Vector& vOut)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, const Vector&, Vector&);
		return getvfunc<OriginalFn>(this, 13)(this, vIn, vOut);
	}
};



class CUserCmd
{
public:
	CUserCmd()
	{
		memset(this, 0, sizeof(*this));
	};
	virtual ~CUserCmd() {};

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;
		CRC32_Init(&crc);

		CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
		CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));

		CRC32_Final(&crc);
		return crc;
	}
	
	int     command_number;     // 0x04 For matching server and client commands for debugging
	int     tick_count;         // 0x08 the tick the client created this command
	Vector  viewangles;         // 0x0C Player instantaneous view angles.
	Vector  aimdirection;       // 0x18
	float   forwardmove;        // 0x24
	float   sidemove;           // 0x28
	float   upmove;             // 0x2C
	int     buttons;            // 0x30 Attack button states
	char    impulse;            // 0x34
	int     weaponselect;       // 0x38 Current weapon id
	int     weaponsubtype;      // 0x3C
	int     random_seed;        // 0x40 For shared random functions
	short   mousedx;            // 0x44 mouse accum in x from create move
	short   mousedy;            // 0x46 mouse accum in y from create move
	bool    hasbeenpredicted;   // 0x48 Client only, tracks whether we've predicted this command at least once
	char    pad_0x4C[0x18];     // 0x4C Current sizeof( usercmd ) =  100  = 0x64
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

enum InvalidatePhysicsBits_t
{
	POSITION_CHANGED = 0x1,
	ANGLES_CHANGED = 0x2,
	VELOCITY_CHANGED = 0x4,
	ANIMATION_CHANGED = 0x8,		// Means cycle has changed, or any other event which would cause render-to-texture shadows to need to be rerendeded
	BOUNDS_CHANGED = 0x10,		// Means render bounds have changed, so shadow decal projection is required, etc.
	SEQUENCE_CHANGED = 0x20,		// Means sequence has changed, only interesting when surrounding bounds depends on sequence																				
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
	Vector& GetAbsOrigin()
	{
		typedef Vector& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 10)(this);
	}
	Vector& GetAbsAngles()
	{
		typedef Vector& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 11)(this);
	}
	Vector& Eyes()
	{
		typedef Vector& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 284)(this);
	}
	void GetWorldSpaceCenter(Vector& vWorldSpaceCenter)
	{
		Vector vMin, vMax;
		this->GetRenderBounds(vMin, vMax);
		vWorldSpaceCenter = this->GetAbsOrigin();
		vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2;
	}
	DWORD* GetModel()
	{
		__asm
		{
			MOV EDI, this
			LEA ECX, [EDI + 0x4]
			MOV EDX, DWORD PTR DS : [ECX]
			CALL[EDX + 0x20]
		}
	}

	bool SetupBones(matrix3x4* matrix, int maxbones, int mask, float time)
	{
		__asm
		{
			MOV EDI, this
			LEA ECX, DWORD PTR DS : [EDI + 0x4]
			MOV EDX, DWORD PTR DS : [ECX]
			PUSH time
			PUSH mask
			PUSH maxbones
			PUSH matrix
			CALL DWORD PTR DS : [EDX + 0x34]
		}
	}

	ClientClass* GetClientClass()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
	}
	bool IsDormant()
	{
		__asm
		{
			MOV EDI, this
			LEA ECX, [EDI + 0x8]
			MOV EDX, DWORD PTR DS : [ecx]
			CALL[EDX + 0x24]
		}
	}
	int GetIndex()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 9)(pNetworkable);
	}


	UINT* GetWearables()
	{
		static int m_hMyWearables = NetVarManager->GetOffset("DT_BaseCombatCharacter", "m_hMyWearables");
		return (UINT*)((DWORD)this + m_hMyWearables);
	}

	void GetRenderBounds(Vector& mins, Vector& maxs)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef void(__thiscall* OriginalFn)(PVOID, Vector&, Vector&);
		getvfunc<OriginalFn>(pRenderable, 17)(pRenderable, mins, maxs);
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
		return getvfunc<OriginalFn>(this, 107)(this); //PROBABLY WRONG FOR CSGO. 100 anyways.
	}
	

	CBaseCombatWeapon * GetActiveWeapon();
	int * GetWeapons();
	int GetSequenceActivity(int sequence);
	void InvalidatePhysicsRecursive(InvalidatePhysicsBits_t nFlag);

	Vector GetCollideableMins();
	Vector GetCollideableMaxs();
	Vector GetViewOffset();
	Vector GetEyePosition();
	Vector EyeAngles();
	Vector GetAimPunchAngle();
	Vector GetOrigin();
	Vector GetBonePosition(int iHitbox);

	Vector GetHitboxPosition(int iHitbox, matrix3x4 * BoneMatrix);

	void SetAbsOrigin(Vector & Angle);
	void EstimateAbsVelocity(Vector & vel);

	bool ComputeHitboxSurroundingBox(Vector * mins, Vector * maxs);
	bool IsAlive();
	bool IsScoped();

	bool & IsSpotted();

	int GetHealth();
	int GetTeam();
	int GetHitboxSet();
	int & GetTickBase();
	int & GetFlags();
	int GetShotsFired();

	BYTE GetLifeState();

	float GetSimulationTime();

	float GetDuckAmount()
	{
		static int m_flDuckAmount = gNetvars->GetOffset("DT_BasePlayer", "m_flDuckAmount");
		return *(float*)((DWORD)this + m_flDuckAmount);
	}

	float GetNextAttack()
	{
		static int m_flNextAttack = NetVarManager->GetOffset("DT_CSPlayer", "m_flNextAttack");
		return *(float*)((uintptr_t)this + m_flNextAttack);
	}
};

class CBaseCombatWeapon : public CBaseEntity
{
public:

	void SendViewModelMatchingSequence(int nSequence)
	{
		typedef void(__thiscall *OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 246)(this, nSequence); 
	}

	float GetNextPrimaryAttack()
	{
		static int m_flNextPrimaryAttack = gNetvars->GetOffset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
		return *(float*)((DWORD)this + m_flNextPrimaryAttack);
	}

	short GetItemDefinitionIndex()
	{
		static int m_iItemDefinitionIndex = gNetvars->GetOffset("DT_BaseAttributableItem", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
		return *(short*)((uintptr_t)this + 0x2FAA);
	}

	void SetItemDefinitionIndex(short index)
	{
		static int m_iItemDefinitionIndex = gNetvars->GetOffset("DT_BaseAttributableItem", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
		*(short*)((uintptr_t)this + 0x2FAA) = index;
	}

	int& GetWorldModel()
	{
		static int m_hWeaponWorldModel = gNetvars->GetOffset("DT_BaseCombatWeapon", "m_hWeaponWorldModel");
		return *(int*)((uintptr_t)this + m_hWeaponWorldModel);
	}
		
	int* GetFallbackPaintKit()
	{
		static int m_nFallbackPaintKit = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_nFallbackPaintKit");
		return (int*)((uintptr_t)this + m_nFallbackPaintKit);
	}

	int* GetWorldModelIndex()
	{
		static int m_iWorldModelIndex = gNetvars->GetOffset("DT_BaseCombatWeapon", "m_iWorldModelIndex");
		return (int*)((uintptr_t)this + m_iWorldModelIndex);
	}


	int &GetSequence()
	{
		static int m_nSequence = NetVarManager->GetOffset("DT_PredictedViewModel", "m_nSequence");
		return *(int*)((uintptr_t)this + m_nSequence);
	}

	void SetSequence(int nSequence)
	{
		static int m_nSequence = NetVarManager->GetOffset("DT_BaseViewModel", "m_nSequence");
		*(int*)((uintptr_t)this + m_nSequence) = nSequence;
	}

	float &GetCycle()
	{
		static int m_flCycle = NetVarManager->GetOffset("DT_CSPlayer", "m_flCycle");
		return *(float*)((uintptr_t)this + m_flCycle);
	}

	void SetCycle(float flCycle)
	{
		static int m_flCycle = NetVarManager->GetOffset("DT_CSPlayer", "m_flCycle");
		*(int*)((uintptr_t)this + m_flCycle) = flCycle;
	}

	void SetModelIndex(int index)
	{
		static int m_nModelIndex = gNetvars->GetOffset("DT_BaseViewModel", "m_nModelIndex");
		*(int*)((uintptr_t)this + m_nModelIndex) = index;
	}

	int GetViewModelIndex()
	{
		static int m_iViewModelIndex = gNetvars->GetOffset("DT_BaseCombatWeapon", "m_iViewModelIndex");
		return *(int*)((uintptr_t)this + m_iViewModelIndex);
	}

	void SetViewModelIndex(int index)
	{
		static int m_iViewModelIndex = gNetvars->GetOffset("DT_BaseCombatWeapon", "m_iViewModelIndex");
		*(int*)((uintptr_t)this + m_iViewModelIndex) = index;
	}

	int* GetEntityQuality()
	{
		static int m_iEntityQuality = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_iEntityQuality");
		return (int*)((DWORD)this + m_iEntityQuality);
	}
	
	int* GetFallbackSeed()
	{
		static int m_nFallbackSeed = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_nFallbackSeed");
		return (int*)((DWORD)this + m_nFallbackSeed);
	}

	float* GetFallbackWear()
	{
		static int m_flFallbackWear = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_flFallbackWear");
		return (float*)((DWORD)this + m_flFallbackWear);
	}

	int* GetFallbackStatTrak()
	{
		static int m_nFallbackStatTrak = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_nFallbackStatTrak");
		return (int*)((DWORD)this + m_nFallbackStatTrak);
	}

	int* GetOwnerXuidLow()
	{
		static int m_OriginalOwnerXuidLow = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
		return (int*)((DWORD)this + m_OriginalOwnerXuidLow);
	}

	int* GetOwnerXuidHigh()
	{
		static int m_OriginalOwnerXuidHigh = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
		return (int*)((DWORD)this + m_OriginalOwnerXuidHigh);
	}

	int* GetAccountID()
	{
		static int m_iAccountID = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_iAccountID");
		return (int*)((DWORD)this + m_iAccountID);
	}

	int* GetItemIDHigh()
	{
		static int m_iItemIDHigh = NetVarManager->GetOffset("DT_BaseAttributableItem", "m_iItemIDHigh");
		return (int*)((uintptr_t)this + m_iItemIDHigh);
	}

	short GetASS()
	{
		static int m_iItemDefinitionIndex = gNetvars->GetOffset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
		return *reinterpret_cast<short*>(reinterpret_cast<std::uintptr_t>(this) + m_iItemDefinitionIndex);
	}

	int GetClip1()
	{
		//DT_BaseCombatWeapon m_iClip1
		static int m_iClip1 = gNetvars->GetOffset("DT_BaseCombatWeapon", "LocalWeaponData", "m_iClip1");
		return *(int*)((DWORD)this + m_iClip1);
	}


	bool IsKnife(int iWeaponID)
	{
		//names wrong for knife t and ct??? nice enum :(
		return (iWeaponID == WEAPON_KNIFE_T || iWeaponID == WEAPON_KNIFE
			|| iWeaponID == 59 || iWeaponID == 41
			|| iWeaponID == 500 || iWeaponID == 505 || iWeaponID == 506
			|| iWeaponID == 507 || iWeaponID == 508 || iWeaponID == 509
			|| iWeaponID == 515);
	}

	float GetInaccuracy() 
	{
		typedef float(__thiscall *OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 482)(this);
	}

	float GetSpread()
	{
		typedef float(__thiscall *OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 452)(this);
	}

	void UpdateAccuracyPenalty()
	{
		typedef void(__thiscall *OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 483)(this);
	}

	//483

	CSWeaponData* GetCSWpnData()
	{
		typedef CSWeaponData*(__thiscall* GetWpnData)(void*);
		static DWORD dwWeaponData = gSignatures.GetClientSignature("55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D");
		static GetWpnData WpnData = (GetWpnData)(dwWeaponData);
		return WpnData(this);
	}
};

class CDoIReallyFuckingNeedThis
{
public:
	inline DWORD GetViewModelOwnerHandle()
	{
		static int m_hOwner = NetVarManager->GetOffset("DT_BaseViewModel", "m_hOwner");
		return *(PDWORD)((DWORD)this + m_hOwner);
	}

	inline int GetModelIndex()
	{
		static int m_nModelIndex = NetVarManager->GetOffset("DT_BaseViewModel", "m_nModelIndex");
		return *(int*)((uintptr_t)this + m_nModelIndex);
	}
};

class CGloves : public CBaseCombatWeapon
{
public:

	//these could go into the main CBaseEntity class but i only need em here so eh
	void PreDataUpdate(int updateType)
	{
		PVOID pNetworkable = (PVOID)((DWORD)(this) + 0x8);
		typedef void(__thiscall* OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(pNetworkable, 6)(pNetworkable, updateType);
	}

	void Release()
	{
		PVOID pNetworkable = (PVOID)((DWORD)(this) + 0x8);
		typedef void(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 1)(pNetworkable);
	}

	void SetDestroyedOnRecreateEntities()
	{
		PVOID pNetworkable = (PVOID)((DWORD)(this) + 0x8);
		typedef void(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 14)(pNetworkable);
	}

	void SetGloveModelIndex(int modelIndex)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int);
		getvfunc<OriginalFn>(this, 75)(this, modelIndex);
	}
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
    virtual void SetColorModulation(float const* blend) = 0;
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
		return getvfunc< GetStudiomodelFn >(this, 32)(this, mod);
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
	virtual const char*             GetName() const = 0;
	virtual const char*             GetTextureGroupName() const = 0;
	virtual PreviewImageRetVal_t    GetPreviewImageProperties(int *width, int *height, ImageFormat *imageFormat, bool* isTranslucent) const = 0;
	virtual PreviewImageRetVal_t    GetPreviewImage(unsigned char *data, int width, int height, ImageFormat imageFormat) const = 0;
	virtual int                     GetMappingWidth() = 0;
	virtual int                     GetMappingHeight() = 0;
	virtual int                     GetNumAnimationFrames() = 0;
	virtual bool                    InMaterialPage(void) = 0;
	virtual    void                 GetMaterialOffset(float *pOffset) = 0;
	virtual void                    GetMaterialScale(float *pScale) = 0;
	virtual IMaterial*              GetMaterialPage(void) = 0;
	virtual IMaterialVar*           FindVar(const char *varName, bool *found, bool complain = true) = 0;
	virtual void                    IncrementReferenceCount(void) = 0;
	virtual void                    DecrementReferenceCount(void) = 0;
	inline void                     AddRef() { IncrementReferenceCount(); }
	inline void                     Release() { DecrementReferenceCount(); }
	virtual int                     GetEnumerationID(void) const = 0;
	virtual void                    GetLowResColorSample(float s, float t, float *color) const = 0;
	virtual void                    RecomputeStateSnapshots() = 0;
	virtual bool                    IsTranslucent() = 0;
	virtual bool                    IsAlphaTested() = 0;
	virtual bool                    IsVertexLit() = 0;
	virtual VertexFormat_t          GetVertexFormat() const = 0;
	virtual bool                    HasProxy(void) const = 0;
	virtual bool                    UsesEnvCubemap(void) = 0;
	virtual bool                    NeedsTangentSpace(void) = 0;
	virtual bool                    NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool                    NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool                    NeedsSoftwareSkinning(void) = 0;
	virtual void                    AlphaModulate(float alpha) = 0;
	virtual void                    ColorModulate(float r, float g, float b) = 0;
	virtual void                    SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
	virtual bool                    GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;
	virtual void                    GetReflectivity(Vector& reflect) = 0;
	virtual bool                    GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
	virtual bool                    IsTwoSided() = 0;
	virtual void                    SetShader(const char *pShaderName) = 0;
	virtual int                     GetNumPasses(void) = 0;
	virtual int                     GetTextureMemoryBytes(void) = 0;
	virtual void                    Refresh() = 0;
	virtual bool                    NeedsLightmapBlendAlpha(void) = 0;
	virtual bool                    NeedsSoftwareLighting(void) = 0;
	virtual int                     ShaderParamCount() const = 0;
	virtual IMaterialVar**          GetShaderParams(void) = 0;
	virtual bool                    IsErrorMaterial() const = 0;
	virtual void                    Unused() = 0;
	virtual float                   GetAlphaModulation() = 0;
	virtual void                    GetColorModulation(float *r, float *g, float *b) = 0;
	virtual bool                    IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;
	virtual IMaterialVar*           FindVarFast(char const *pVarName, unsigned int *pToken) = 0;
	virtual void                    SetShaderAndParams(KeyValues *pKeyValues) = 0;
	virtual const char*             GetShaderName() const = 0;
	virtual void                    DeleteIfUnreferenced() = 0;
	virtual bool                    IsSpriteCard() = 0;
	virtual void                    CallBindProxy(void *proxyData) = 0;
	virtual void                    RefreshPreservingMaterialVars() = 0;
	virtual bool                    WasReloadedFromWhitelist() = 0;
	virtual bool                    SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
	virtual int                     GetReferenceCount() const = 0;
};

class IMaterialSystem
{
public:
	IMaterial* FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL)
	{
		typedef IMaterial *(__thiscall *OriginalFn)(PVOID, const char *, const char *, bool, const char *);
		return getvfunc<OriginalFn>(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}
};


class IVModelRender
{
public:
	void ForcedMaterialOverride(IMaterial* material, OverrideType_t type = OVERRIDE_NORMAL, int idk = NULL)
	{
		typedef void(__thiscall* Fn)(void*, IMaterial*, OverrideType_t, int);
		return getvfunc<Fn>(this, 1)(this, material, type, idk);
	}
};


struct ModelRenderInfo_t
{
	Vector origin;
	Vector angles;
	char pad[0x4];
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


enum AnalogCode_t //needed but not rly
{

};

class EngineClient
{
public:
	void GetScreenSize(int& width, int& height)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int&, int&);
		return getvfunc<OriginalFn>(this, 5)(this, width, height);
	}
	bool GetPlayerInfo(int ent_num, player_info_t* pinfo)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID, int, player_info_t*);
		return getvfunc<OriginalFn>(this, 8)(this, ent_num, pinfo);
	}
	int GetPlayerForUserID(int userID)
	{
		typedef int(__thiscall* OriginalFn)(PVOID, int);
		return getvfunc< OriginalFn >(this, 9)(this, userID);
	}
	bool Con_IsVisible(void)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 11)(this);
	}
	int GetLocalPlayer(void)
	{
		typedef int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 12)(this);
	}
	float Time(void)
	{
		typedef float(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 14)(this);
	}
	void GetViewAngles(Vector& va)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, Vector& va);
		return getvfunc<OriginalFn>(this, 18)(this, va);
	}
	void SetViewAngles(Vector& va)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, Vector& va);
		return getvfunc<OriginalFn>(this, 19)(this, va);
	}
	bool IsInGame(void)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 26)(this);
	}
	bool IsConnected(void)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 27)(this);
	}
	bool IsDrawingLoadingImage(void)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 28)(this);
	}
	const matrix3x4& WorldToScreenMatrix(void)
	{
		typedef const matrix3x4& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 37)(this);
	}
	bool IsTakingScreenshot(void)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 87)(this);
	}
	inline DWORD* GetNetChannelInfo(void)
	{
		typedef DWORD* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 78)(this);
	}
	void ClientCmd_Unrestricted(const char* chCommandString)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, const char*);
		return getvfunc<OriginalFn>(this, 114)(this, chCommandString);
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
	int			x, x_old;
	int			y, y_old;
	int			width, width_old;
	int			height, height_old;
	bool		m_bOrtho;
	float		m_OrthoLeft;
	float		m_OrthoTop;
	float		m_OrthoRight;
	float		m_OrthoBottom;
	bool		m_bCustomViewMatrix;
	matrix3x4	m_matCustomViewMatrix;
	char		pad_0x68[0x48];
	float		fov;
	float		fovViewmodel;
	Vector		origin;
	Vector		angles;
	float		zNear;
	float		zFar;
	float		zNearViewmodel;
	float		zFarViewmodel;
	float		m_flAspectRatio;
	float		m_flNearBlurDepth;
	float		m_flNearFocusDepth;
	float		m_flFarFocusDepth;
	float		m_flFarBlurDepth;
	float		m_flNearBlurRadius;
	float		m_flFarBlurRadius;
	int			m_nDoFQuality;
	int			m_nMotionBlurMode;
	float		m_flShutterTime;
	Vector		m_vShutterOpenPosition;
	Vector		m_shutterOpenAngles;
	Vector		m_vShutterClosePosition;
	Vector		m_shutterCloseAngles;
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;
	int			m_EdgeBlur;
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
		getvfunc<OriginalFn>(this, 15)(this, r, g, b, a);
	}
	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 16)(this, x0, y0, x1, y1);
	}
	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 18)(this, x0, y0, x1, y1);
	}
	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 19)(this, x0, y0, x1, y1);
	}
	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long);
		getvfunc<OriginalFn>(this, 23)(this, font);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 25)(this, r, g, b, a);
	}
	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int);
		getvfunc<OriginalFn>(this, 26)(this, x, y);
	}
	void DrawPrintText(const wchar_t* text, int textLen)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, const wchar_t*, int, int);
		return getvfunc<OriginalFn>(this, 28)(this, text, textLen, 0);
	}
	void GetScreenSize(int& width, int& height) //This works in full screen, engine doesn't.
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int&, int&);
		return getvfunc<OriginalFn>(this, 44)(this, width, height);
	}
	unsigned long CreateFont()
	{
		typedef unsigned int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 71)(this);
	}
	void DrawTexturedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		return getvfunc<OriginalFn>(this, 41)(this, x0, y0, x1, y1);
	}
	void SetFontGlyphSet(unsigned long& font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int);
		getvfunc<OriginalFn>(this, 72)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}
	void GetTextSize(unsigned long font, const wchar_t* text, int& wide, int& tall)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const wchar_t*, int&, int&);
		getvfunc<OriginalFn>(this, 79)(this, font, text, wide, tall);
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

class __declspec(align(16))VectorAligned : public Vector
{
public:
	VectorAligned& operator=(const Vector &vOther)
	{
		Init(vOther.x, vOther.y, vOther.z);
		return *this;
	}
	float w;
};
class IHandleEntity;

struct Ray_t
{
	VectorAligned  m_Start;    // starting point, centered within the extents
	VectorAligned  m_Delta;    // direction + length of the ray
	VectorAligned  m_StartOffset;    // Add this to m_Start to get the actual ray start
	VectorAligned  m_Extents;    // Describes an axis aligned box extruded along a ray
	const matrix3x4 *m_pWorldAxisTransform;
	//const matrix3x4_t *m_pWorldAxisTransform;
	bool    m_IsRay;    // are the extents zero?
	bool    m_IsSwept;    // is delta != 0?



	void Init(const Vector& vecStart, const Vector& vecEnd)
	{
		m_Delta = vecEnd - vecStart;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents.x = m_Extents.y = m_Extents.z = 0.0f;
		m_IsRay = true;

		m_StartOffset.x = m_StartOffset.y = m_StartOffset.z = 0.0f;

		m_Start = vecStart;
	}

	void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs) {
		m_Delta.x = end.x - start.x;
		m_Delta.y = end.y - start.y;
		m_Delta.z = end.z - start.z;

		m_pWorldAxisTransform = nullptr;
		m_IsSwept = m_Delta.LengthSqr() != 0;

		m_Extents.x = maxs.x - mins.x;
		m_Extents.y = maxs.y - mins.y;
		m_Extents.z = maxs.z - mins.z;

		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSqr() < 1e-6);

		m_StartOffset.x = maxs.x + mins.x;
		m_StartOffset.y = maxs.y + mins.y;
		m_StartOffset.z = maxs.z + mins.z;

		m_StartOffset *= 0.5f;

		m_Start.x = start.x + m_StartOffset.x;
		m_Start.y = start.y + m_StartOffset.y;
		m_Start.z = start.z + m_StartOffset.z;

		m_StartOffset *= -1.0f;
	}

private:
};

struct csurface_t
{
	const char *name;
	short surfaceProps;
	unsigned short flags;
};

struct cplane_t
{
	Vector normal;
	float dist;
	byte type;
	byte signbits;
	byte pad[2];
};
struct trace_t
{
	Vector start;
	Vector end;
	cplane_t plane;
	float flFraction;
	int contents;
	WORD dispFlags;
	bool allsolid;
	bool startSolid;
	float fractionLeftSolid;
	csurface_t surface;
	int hitGroup;
	short physicsBone;
	CBaseEntity* m_pEnt;
	int hitbox;
	bool DidHit() const { return flFraction < 1 || allsolid || startSolid; };
	inline bool IsVisible() const
	{
		return flFraction > 0.97f;
	}

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
	virtual TraceType_t GetTraceType() const = 0;
};

class CTraceFilter2 : public ITraceFilter
{
public:
	explicit CTraceFilter2(CBaseEntity* entity, TraceType_t tracetype = TRACE_EVERYTHING)
	{
		pSkip1 = entity;
	}

	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return (pEntityHandle != pSkip1);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip1;
};
class CTraceFilter : public ITraceFilter
{
public:
	/*	explicit CTraceFilter(CBaseEntity* entity, TraceType_t tracetype = TRACE_EVERYTHING)
		{
			pSkip1 = entity;
		}*/

	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return (pEntityHandle != pSkip1);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip1;
};
class CTraceFilterOneEntity : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return (pEntityHandle == pEntity);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pEntity;
};
class CTraceFilterOneEntity2 : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return (pEntityHandle == pEntity);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_ENTITIES_ONLY;
	}

	void* pEntity;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
	CTraceFilterSkipTwoEntities(void *pPassEnt1, void *pPassEnt2)
	{
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity(void *pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == passentity1 || pEntityHandle == passentity2);
	}

	virtual TraceType_t    GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void *passentity1;
	void *passentity2;
};

class CTraceEntity : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip1);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_ENTITIES_ONLY;
	}

	void* pSkip1;
};
class CTraceWorldOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return false;
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip1;
};

class IEngineTrace
{
public:	  //We really only need this I guess...
	void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace)//5
	{
		typedef void(__thiscall* TraceRayFn)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
		return getvfunc<TraceRayFn>(this, 5)(this, ray, fMask, pTraceFilter, pTrace);
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

class IPlayerInfoManager
{
public:
	CGlobals* GetGlobalVars()
	{
		typedef CGlobals* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 1)(this);
	}
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
	IMaterialSystem* MaterialSystem;
	IVModelRender* ModelRender;
	IVModelInfo* ModelInfo;
	ICvar* Cvar;
	IEngineTrace* EngineTrace;
	IRenderView* RenderView;
	IDemoPlayer* DemoPlayer;
	IPlayerInfoManager* PlayerInfo;
};

using OverrideViewFn = int(__stdcall*)(CViewSetup*);
extern OverrideViewFn oView;

using ViewmodelFn = float(__stdcall*)();
extern ViewmodelFn oViewModel;

using CreateMoveFn = bool(__thiscall*)(PVOID, float, CUserCmd*);
extern CreateMoveFn bReturn;

using ShouldDrawFogFn = bool(__thiscall*)(PVOID);
extern ShouldDrawFogFn oFog;

using PanelsFn = void(__thiscall*)(PVOID, unsigned int, bool, bool);
extern PanelsFn oPanel;

using FrameStageFn = void(__thiscall*)(PVOID, ClientFrameStage_t);
extern FrameStageFn oFrameStage;

using DrawModelExecuteFn = void(__thiscall *)(PVOID, PVOID, PVOID, const ModelRenderInfo_t&, matrix3x4*);
extern DrawModelExecuteFn oDrawModel;

using LevelInitPreEntity = void(__stdcall *)(const char*);
extern LevelInitPreEntity oLevelInit;

using LevelInitPostEntity = void(__stdcall *)( );
extern LevelInitPostEntity oLevelPost;

using IsPlayingBackFn = bool(__thiscall*)(PVOID);
extern IsPlayingBackFn oIsPlayingBack;

using IsPlayingTimeDemoFn = bool(__thiscall*)(PVOID);
extern IsPlayingTimeDemoFn oIsPlayingTimeDemo;

extern CInterfaces gInts;
extern CPlayerVariables gVars;
extern COffsets gOffsets;
extern CConvars gConvars;