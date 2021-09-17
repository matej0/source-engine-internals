#pragma once
#include "SDK.h"
#include <deque>
#include "Math.h"





struct BacktrackingData
{
	float     m_flSimulationTime;
	Vector    m_vecHitboxPosition;
	matrix3x4 m_Matrix[128];
};
extern BacktrackingData Data[64][12];




bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
void __fastcall Hooked_FrameStageNotify(PVOID ClientMode, int edx, ClientFrameStage_t Frame);
void __stdcall Hooked_OverrideView(CViewSetup* pSetup);
bool __fastcall Hooked_ShouldDrawFog(PVOID ecx, PVOID edx);
float __stdcall Hooked_GetViewmodelFov();

