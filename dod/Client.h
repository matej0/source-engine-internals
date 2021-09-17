#pragma once
#include "SDK.h"
#include <deque>

#define TICK_INTERVAL			( gInts.Globals->interval_per_tick)
#define TIME_TO_TICKS( t )		( (int)( 0.5f + (float)( t ) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL * (float)( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		( -1 )

class RequiredMath
{
public:
	void CorrectMovement(Vector old_angles, CUserCmd* cmd, float old_forwardmove, float old_sidemove)
	{
		float delta_view;
		float f1;
		float f2;

		if (old_angles.y < 0.f)
			f1 = 360.0f + old_angles.y;
		else
			f1 = old_angles.y;

		if (cmd->viewangles.y < 0.0f)
			f2 = 360.0f + cmd->viewangles.y;
		else
			f2 = cmd->viewangles.y;

		if (f2 < f1)
			delta_view = abs(f2 - f1);
		else
			delta_view = 360.0f - abs(f1 - f2);

		delta_view = 360.0f - delta_view;

		cmd->forwardmove = cos(DEG2RAD(delta_view)) * old_forwardmove + cos(DEG2RAD(delta_view + 90.f)) * old_sidemove;
		cmd->sidemove = sin(DEG2RAD(delta_view)) * old_forwardmove + sin(DEG2RAD(delta_view + 90.f)) * old_sidemove;
	}


	void AngleVectors(const Vector &angles, Vector& forward)
	{
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	float DistancePointToLine(Vector Point, Vector LineOrigin, Vector Dir)
	{
		auto PointDir = Point - LineOrigin;

		auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
		if (TempOffset < 0.000001f)
			return FLT_MAX;

		auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

		return (Point - PerpendicularPoint).Length();
	}
};
extern RequiredMath gMath;

struct Data
{
	float flSimulationTime;
	Vector vecHead;
	matrix3x4 Matrix[128];
};
extern Data LagCompensationData[64][12];

struct Incoming_Sequence_Record
{
	Incoming_Sequence_Record(int in_reliable, int out_reliable, int in_sequence, float realtime)
	{
		in_reliable_state = in_reliable;
		out_reliable_state = out_reliable;
		in_sequence_num = in_sequence;

		time = realtime;
	}

	int in_reliable_state;
	int out_reliable_state;
	int in_sequence_num;

	float time;
};

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
void __fastcall Hooked_FrameStageNotify(PVOID ClientMode, int edx, ClientFrameStage_t Frame);
void __stdcall Hooked_OverrideView(CViewSetup* pSetup);
void __fastcall Hooked_EnableWorldFog(PVOID ecx, PVOID edx);
void __stdcall Hooked_EnableSkyBoxFog();
void __stdcall Hooked_DoEnginePostProcessing();
void __stdcall Hooked_LevelInitPreEntity(const char* map);
void __stdcall Hooked_LevelInitPostEntity();
int __fastcall Hooked_SendDatagram(CNetChannel* ecx, PVOID edx, PVOID data);
bool __fastcall Hooked_ShouldDrawFog(PVOID ecx, PVOID edx);

void RemoveCommandLimitations();

//some server lagger. tried to reverse the cheat but couldnt figure it out.

/*int __thiscall sub_10002710(int this)
{
	int v1; // esi

	v1 = this;
	*(_DWORD *)(this + 0x14) = 0;
	*(_DWORD *)(this + 0x18) = 0;
	*(_DWORD *)(this + 0x1C) = -1;
	*(_DWORD *)(this + 0x20) = 0;
	*(_WORD *)(this + 0x24) = 256;
	*(_DWORD *)(this + 0x28) = 0;
	*(_DWORD *)(this + 0x2C) = 0;
	*(_DWORD *)(this + 0x30) = 0;
	*(_DWORD *)(this + 0x34) = -1;
	*(_DWORD *)(this + 0x38) = 0;
	*(_WORD *)(this + 0x3C) = 256;
	*(_DWORD *)(this + 0x40) = 0;
	MEMORY[0x20002FE0]();
	return v1;
}*/

/*
if ( GetAsyncKeyState(0x4A) )                 // on keypress J
  {
	sub_10002710((int)&v8);

	v10 = &v15;
	v11 = 2048;
	v12 = 0x4000;
	v14 = 0;
	v9 = 500;
	v13 = 500;

	pNetChannelInfo = (*(int (**)(void))(*(_DWORD *)pEngine + 288))();

	if ( pNetChannelInfo )
	{
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	  (*(void (__thiscall **)(int, char *, _DWORD))(*(_DWORD *)pNetChannelInfo + 144))(pNetChannelInfo, &v8, 0);
	}
  }

*/