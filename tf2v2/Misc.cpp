#include "Misc.h"
CMisc gMisc;

void CMisc::RemoveDisguise()
{
	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity)
			continue;

		if (pEntity->GetCond() & TFCond_Disguised)
			pEntity->GetCond() &= ~TFCond_Disguised;
	}

}

void CMisc::SimulateLag(CUserCmd* pCommand)
{
	if (!GetAsyncKeyState(VK_XBUTTON1))
		return;

	CNetChannel* pNetChannel = gInts.Engine->GetNetChannelInfo();

	if (!pNetChannel)
		return;

	if (pNetChannel->m_nChokedPackets <= gPlayerVars.iFakeLagAmount)
		*gPlayerVars.bSendPacket = false;
	else
		*gPlayerVars.bSendPacket = true;
}

void CMisc::AutoJump(CBaseEntity* pLocal, CUserCmd* pCommand)
{
	if (!gPlayerVars.bBhop)
		return;

	//credits: gir489
	if (pLocal->GetClassNum() == TF2_Scout)
	{
		static bool bJumpReleased;
		if (pCommand->buttons & IN_JUMP)
		{
			if (!bJumpReleased)
			{
				if (!(pLocal->GetFlags() & FL_ONGROUND))
				{
					pCommand->buttons &= ~IN_JUMP;
				}
			}
			else
			{
				bJumpReleased = false;
			}
		}
		else if (!bJumpReleased)
		{
			bJumpReleased = true;
		}

	}
	else
	{
		if (!(pLocal->GetFlags() & FL_ONGROUND) && pCommand->buttons & IN_JUMP)
			pCommand->buttons &= ~IN_JUMP;
	}
}

void CMisc::ComputeMove(CBaseEntity* pLocal)
{
	if (!pLocal || !pLocal->IsAlive())
		return;

	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity == pLocal)
			continue;

		Vector MoveToAngles;
		Vector CurrentOrigin = pEntity->GetOrigin();
		Vector LastOrigin    = Vector();
		Vector Difference    = CurrentOrigin - LastOrigin;

		Vector Velocity = Difference; // name change and addition to clear up confusion due to posts below

		float Speed = Difference.Length();

		Difference.Normalize();

		MoveToAngles = Difference.ToEulerAngles();

		if (CurrentOrigin != LastOrigin)
			LastOrigin = CurrentOrigin;
	}
}