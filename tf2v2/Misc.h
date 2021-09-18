#pragma once
#include "SDK.h"
#include "Util.h"

class CMisc
{
public:
	void RemoveDisguise();
	void SimulateLag(CUserCmd * pCommand);
	void AutoJump(CBaseEntity * pLocal, CUserCmd * pCommand);
	void ComputeMove(CBaseEntity * pLocal);
};
extern CMisc gMisc;