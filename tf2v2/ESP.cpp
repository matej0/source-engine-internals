#include "ESP.h"
#include "CDrawManager.h"
#include "CMenu.h"
#include "Aimbot.h"

CEsp gESP;
CRadar gRadar;

void CEsp::Think(CBaseEntity* pLocal)
{
	//gESP.m_Players.clear();
	//gESP.m_Buildings.clear();

	for (int i = 1; i <= gInts.EntList->GetHighestEntityIndex(); i++)
	{
		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity || pEntity == pLocal)
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam() && !gPlayerVars.bTeammates)
			continue;

		if (!pEntity->IsAlive() || pEntity->IsDormant())
			continue;

		Players(pLocal, pEntity);
		Buildings(pLocal, pEntity);

		//this is completely unneccessary but i just wanted to see if it would work.
		/*if (pEntity->IsPlayer()) //if entity is player, populate player vector.
		{
			IEspPlayerData playerData;
			playerData.m_pEntity = pEntity;
			playerData.m_Index = i;
			gESP.m_Players.emplace_back(playerData);
		}
		else if (pEntity->IsBuilding()) //same as above.
		{
			IEspBuildingsData buildingsData;
			buildingsData.m_pEntity = pEntity;
			buildingsData.m_Index = i;
			gESP.m_Buildings.emplace_back(buildingsData);
		}
		else
		{
			if (gPlayerVars.bOther)
			{
				static DWORD dwClr;
				switch (pEntity->GetTeam())
				{
				case AXIS:
					dwClr = COLOR_DOD_GREEN; break;
				case WEHRMACHT:
					dwClr = COLOR_DOD_RED; break;
				}


				Vector vS;

				if (!gDrawManager.WorldToScreen(pEntity->GetAbsOrigin(), vS))
					return;

				switch (pEntity->GetClientClass()->iClassID)
				{
				case CTFGrenadePipebombProjectile:
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, vS.x, vS.y, dwClr, "<->"); break;
				case CTFProjectile_Rocket:
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, vS.x, vS.y, dwClr, "[==]"); break;
				case CCaptureFlag:
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, vS.x, vS.y, dwClr, "Capture"); break;
				case CTFProjectile_Arrow:
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, vS.x, vS.y, dwClr, "[->]"); break;
				}
			}
		}*/

	}
}

void Skeleton(CBaseEntity* pEntity)
{
	if (!gPlayerVars.bAimbot)
		return; //we use the aimbot matrix.

	int parent;
	studiohdr_t*  pHdr;
	mstudiobone_t* pStudioBone;
	matrix3x4     matrix[128];

	Vector vBonePos, vParentPos, vBonePosScreen, vParentPosScreen;

	const DWORD* dwModel = pEntity->GetModel();

	if (!dwModel)
		return;

	pHdr = gInts.ModelInfo->GetStudiomodel(dwModel);

	if (!pHdr)
		return;

	//get bone matrix.
	//if (!pEntity->SetupBones(matrix, 128, 0x0007FF00, gInts.Globals->curtime))
	//	return;

	for (int i{ }; i < pHdr->numbones; ++i)
	{
		pStudioBone = pHdr->GetBone(i);
		if (!pStudioBone || !(pStudioBone->flags & 256))
			continue;

		//get parent bone.
		parent = pStudioBone->parent;
		if (parent == -1)
			continue;

		vBonePos = { gAim.m_Matrix[pEntity->GetIndex()][i][0][3], gAim.m_Matrix[pEntity->GetIndex()][i][1][3], gAim.m_Matrix[pEntity->GetIndex()][i][2][3] };
		vParentPos = { gAim.m_Matrix[pEntity->GetIndex()][parent][0][3], gAim.m_Matrix[pEntity->GetIndex()][parent][1][3], gAim.m_Matrix[pEntity->GetIndex()][parent][2][3] };

		if (gDrawManager.WorldToScreen(vBonePos, vBonePosScreen) && gDrawManager.WorldToScreen(vParentPos, vParentPosScreen))
			gDrawManager.DrawLine(vBonePosScreen.x, vBonePosScreen.y, vParentPosScreen.x, vParentPosScreen.y, COLORCODE(255, 255, 255, 200));
	}
}


void CEsp::Players(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	if (!gPlayerVars.bEspPlayers)
		return;

	if (!pEntity || !pEntity->IsPlayer())
		return; 

	int x, y, w, h;

	if (!gDrawManager.CalculatePlayerBounds(pEntity, x, y, w, h))
		return;

	player_info_t info;
	gInts.Engine->GetPlayerInfo(pEntity->GetIndex(), &info);

	int nOffset = 0;
	static DWORD dwPlayerColor;

	if (gPlayerVars.bTeamColored)
	{
		switch (pEntity->GetTeam())
		{
		case AXIS:
			dwPlayerColor = COLOR_DOD_GREEN; break;
		case WEHRMACHT:
			dwPlayerColor = COLOR_DOD_RED; break;
		}
	}
	else
	{
		dwPlayerColor = COLOR_WHITE;
	}

	if (gPlayerVars.bBox)
	{
		gDrawManager.OutlineRect(x - 1, y - 1, w + 2, h + 2, COLORCODE(0, 0, 0, 255));
		gDrawManager.OutlineRect(x, y, w, h, dwPlayerColor);
		gDrawManager.OutlineRect(x + 1, y + 1, w - 2, h - 2, COLORCODE(0, 0, 0, 255));
	}

	if (gPlayerVars.bName)
	{
		//wchar_t szString[1024];
		//memset(szString, 0, sizeof(wchar_t) * 1024);

		//if (MultiByteToWideChar(CP_UTF8, 0, info.name, MAX_PLAYER_NAME_LENGTH, szString, 1024))
		//	gDrawManager.DrawString(xP, yP - 15, dColor, szString);
		//else
		gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, (x + (w / 2)), y - 15, COLOR_WHITE, "%s", info.name);
	}

	if (gPlayerVars.bHealth)
	{
		int iMaxHP = pEntity->GetMaxHealth();
		int iHP = pEntity->GetHealth();

		int iGreen = (255 * pEntity->GetHealth()) / iMaxHP;
		int iRed = 255 - iGreen;

		if (iHP > iMaxHP)
		{
			iHP = iMaxHP;
			iGreen = 255, iRed = 0;
		}
		int iFill = (int)std::round(h * iHP / iMaxHP);
		gDrawManager.DrawRect(x - 8, y - 1, 5, h + 2, COLORCODE(25, 25, 25, 230));
		gDrawManager.DrawRect(x - 7, y + h - iFill, 3, iFill, COLORCODE(iRed, iGreen, 0, 240));
	}

	if (gPlayerVars.bWeapon)
	{
		gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, (x + (w / 2)), y + h, COLOR_WHITE, gDrawManager.szCurrentWeaponName(pEntity));
	}

	if (gPlayerVars.bSkeleton)
		Skeleton(pEntity);

	if (pEntity->GetClassNum() == TF2_Medic)
	{
		CBaseCombatWeapon* pMedigun = pEntity->GetWeaponFromSlot(TF_WEAPONSLOT_SECONDARY);

		if (pMedigun)
		{
			gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 3, y + nOffset, COLOR_WHITE, "%.0f%%", pMedigun->GetUberCharge() * 100);
			nOffset += 15;
		}
	}

	if (gPlayerVars.bConditions)
	{
		std::vector<std::string> rgConds = { };

		//wow.
		if (pEntity->GetCond() & TFCond_Ubercharged || pEntity->GetCond() & TFCond_UberchargeFading)
			rgConds.push_back("UBERCHARGED");

		if (pEntity->GetCond() & TFCond_Cloaked)
			rgConds.push_back("CLOAKED");

		if (pEntity->GetCond() & TFCond_Zoomed)
			rgConds.push_back("ZOOMED");

		if (pEntity->GetCond() & TFCond_Jarated)
			rgConds.push_back("JARATE");

		if (pEntity->GetCond() & TFCond_Milked)
			rgConds.push_back("MILK");

		if (pEntity->GetCond() & TFCond_Disguised)
			rgConds.push_back("DISGUISED");

		if (pEntity->GetCond() & TFCond_Disguising)
			rgConds.push_back("DISGUISING");

		if (pEntity->GetCond() & TFCond_Bonked)
			rgConds.push_back("BONKED");

		if (pEntity->GetCond() & TFCond_Kritzkrieged)
			rgConds.push_back("KRITZ");

		if (pEntity->GetCond() & TFCond_Overhealed)
			rgConds.push_back("OVERHEALED");

		for (size_t i = 0; i < rgConds.size(); i++)
		{
			gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 3, y + (i * 15) + nOffset, COLOR_WHITE, rgConds[i].c_str());
			nOffset += 15;
		}
	}
}

void CEsp::Buildings(CBaseEntity* pLocal, CBaseEntity* pEntity)
{
	//this function is a mess if ive ever seen one.
	if (!gPlayerVars.bEspBuildings)
		return;

	if (!pEntity || !pEntity->IsBuilding())
		return;

	int x, y, w, h;
	if (!gDrawManager.CalculateBounds(pEntity, x, y, w, h))
		return;

	static DWORD dwPlayerColor;

	if (gPlayerVars.bTeamColored)
	{
		switch (pEntity->GetTeam())
		{
		case AXIS:
			dwPlayerColor = COLOR_DOD_GREEN; break;
		case WEHRMACHT:
			dwPlayerColor = COLOR_DOD_RED; break;
		}
	}
	else
	{
		dwPlayerColor = COLOR_WHITE;
	}

	switch (pEntity->GetClientClass()->iClassID)
	{
	case CObjectSentrygun:
	{
		CTFObjectSentryGun* pSentryGun = reinterpret_cast<CTFObjectSentryGun*>(pEntity);

		if (pSentryGun && pSentryGun->IsAlive())
		{

			if (gPlayerVars.bBoxBuildings)
			{
				gDrawManager.OutlineRect(x - 1, y - 1, w + 2, h + 2, COLORCODE(0, 0, 0, 255));
				gDrawManager.OutlineRect(x, y, w, h, dwPlayerColor);
				gDrawManager.OutlineRect(x + 1, y + 1, w - 2, h - 2, COLORCODE(0, 0, 0, 255));
			}


			if (gPlayerVars.bNameBuildings)
				gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, x + (w / 2), (y - 15), COLOR_WHITE, "Sentry");

			if (gPlayerVars.bBuildingLevel)
			{
				if (!pSentryGun->IsBuilding())
				{
					gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 3, y, COLOR_WHITE, "Level: %i", pSentryGun->GetLevel());
				}
			}

			if (gPlayerVars.bHealthBuildings)
			{
				int iHP = pSentryGun->GetHealth();
				int iMaxHP = pSentryGun->GetMaxHealth();
				int iGreen = (255 * iHP) / iMaxHP;
				int iRed = 255 - iGreen;

				gDrawManager.DrawRect(x - 1, y + h + 4, w + 2, 5, COLORCODE(25, 25, 25, 230));
				gDrawManager.DrawRect(x, y + h + 5, (pSentryGun->GetHealth() * w) / pSentryGun->GetMaxHealth(), 3, COLORCODE(iRed, iGreen, 0, 240));
			}
		}
	} break;
	case CObjectDispenser:
	{
		CTFObjectDispenser* pDispenser = reinterpret_cast<CTFObjectDispenser*>(pEntity);

		if (pDispenser && pDispenser->IsAlive())
		{
			if (gPlayerVars.bBoxBuildings)
			{
				gDrawManager.OutlineRect(x - 1, y - 1, w + 2, h + 2, COLORCODE(0, 0, 0, 255));
				gDrawManager.OutlineRect(x, y, w, h, dwPlayerColor);
				gDrawManager.OutlineRect(x + 1, y + 1, w - 2, h - 2, COLORCODE(0, 0, 0, 255));
			}
			if (gPlayerVars.bNameBuildings)
				gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, x + (w / 2), (y - 15), COLOR_WHITE, "Dispenser");

			if (gPlayerVars.bBuildingLevel)
			{
				if (!pDispenser->IsBuilding())
					gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 3, y, COLOR_WHITE, "Level: %i", pDispenser->GetLevel());
			}

			if (gPlayerVars.bHealthBuildings)
			{
				int iHP = pDispenser->GetHealth();
				int iMaxHP = pDispenser->GetMaxHealth();
				int iGreen = (255 * iHP) / iMaxHP;
				int iRed = 255 - iGreen;

				gDrawManager.DrawRect(x - 1, y + h + 4, w + 2, 5, COLORCODE(25, 25, 25, 230));
				gDrawManager.DrawRect(x, y + h + 5, (pDispenser->GetHealth() * w) / pDispenser->GetMaxHealth(), 3, COLORCODE(iRed, iGreen, 0, 240));
			}
		}
	} break;
	case CObjectTeleporter:
	{
		CTFObjectTeleporter* pTeleporter = reinterpret_cast<CTFObjectTeleporter*>(pEntity);

		if (pTeleporter && pTeleporter->IsAlive())
		{
			if (gPlayerVars.bBoxBuildings)
			{
				gDrawManager.OutlineRect(x - 1, y - 1, w + 2, h + 2, COLORCODE(0, 0, 0, 255));
				gDrawManager.OutlineRect(x, y, w, h, dwPlayerColor);
				gDrawManager.OutlineRect(x + 1, y + 1, w - 2, h - 2, COLORCODE(0, 0, 0, 255));
			}

			if (gPlayerVars.bNameBuildings)
				gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, x + (w / 2), (y - 15), COLOR_WHITE, "Teleporter");

			if (gPlayerVars.bBuildingLevel)
			{
				if (!pTeleporter->IsBuilding())
				{
					gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 3, y, COLOR_WHITE, "Level: %i", pTeleporter->GetLevel());
				}
			}

			if (gPlayerVars.bHealthBuildings)
			{
				int iHP = pTeleporter->GetHealth();
				int iMaxHP = pTeleporter->GetMaxHealth();
				int iGreen = (255 * iHP) / iMaxHP;
				int iRed = 255 - iGreen;

				gDrawManager.DrawRect(x - 1, y + h + 4, w + 2, 5, COLORCODE(25, 25, 25, 230));
				gDrawManager.DrawRect(x, y + h + 5, (pTeleporter->GetHealth() * w) / pTeleporter->GetMaxHealth(), 3, COLORCODE(iRed, iGreen, 0, 240));
			}

			if (gPlayerVars.bNameBuildings)
			{
				switch (pTeleporter->GetObjectMode())
				{
				case TFObjectMode_Entrance:
				{
					gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 3, y + 15, COLOR_WHITE, "Tele in");

				} break;
				case TFObjectMode_Exit:
				{
					gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 3, y + 15, COLOR_WHITE, "Tele out");
				}break;
				}
			}
		}
	} break;

	}
	
	
}

void CEsp::FakeLagIndicator(CBaseEntity* pLocal)
{
	if (!pLocal || !pLocal->IsAlive())
		return;

	if (!pLocal->GetActiveWeapon())
		return;

	int x, y;
	gInts.Engine->GetScreenSize(x, y);

	x /= 2;
	y /= 2;

	if (GetAsyncKeyState(VK_XBUTTON1))
	{
		gDrawManager.DrawRect(x - 30, y + 25, 60, 6, COLORCODE(25, 25, 25, 200));

		CNetChannel* pNetChannel = gInts.Engine->GetNetChannelInfo();

		int w;

		if (pNetChannel->m_nChokedPackets)
			w = 60 * pNetChannel->m_nChokedPackets / gPlayerVars.iFakeLagAmount;
		else
			w = 0;

		gDrawManager.DrawRect(x - 30, y + 25, w, 6, COLORCODE(255, 120, 25, 250));
	}
}

void CEsp::SpectatorsList(CBaseEntity* pLocal)
{
	if (!gPlayerVars.bSpecList)
		return;

	int x, y;
	gInts.Engine->GetScreenSize(x, y);
	x /= 2;

	std::vector< std::string > spectators;
	int h = 25;

	for (int i{ 1 }; i <= gInts.Engine->GetMaxClients(); ++i)
	{
		CBaseEntity* player = GetBaseEntity(i);

		if (!player)
			continue;

		if (player == pLocal)
			continue;

		if (player->IsDormant())
			continue;

		if (player->IsAlive())
			continue;

		if (player->GetObserverTarget() != pLocal)
			continue;

		if (player->GetTeam() != pLocal->GetTeam())
			continue;

		player_info_t info;

		if (!gInts.Engine->GetPlayerInfo(i, &info))
			continue;

		spectators.push_back(std::string(info.name).substr(0, 24));
	}

	size_t total_size = spectators.size() * (h - 1);

	for (size_t i{ }; i < spectators.size(); ++i)
	{
		const std::string& name = spectators[i];
		gDrawManager.DrawStringA(gDrawManager.GetFont(), true, x, 100 + (total_size / 2) + (i * (h - 1)), COLORCODE(255, 255, 255, 255), name.c_str());
	}
}
void CRadar::DrawRadarPoint(Vector vOriginX, Vector vOriginY, Vector qAngle, CBaseEntity *pBaseEntity, DWORD dwTeamColor)
{
	if (!gPlayerVars.bRadar)
		return;

	float flDeltaX = vOriginX.x - vOriginY.x;
	float flDeltaY = vOriginX.y - vOriginY.y;

	float flAngle = qAngle.y;

	float flYaw = (flAngle)* (PI / 180.0);
	float flMainViewAngles_CosYaw = cos(flYaw);
	float flMainViewAngles_SinYaw = sin(flYaw);

	// rotate
	float x = flDeltaY * (-flMainViewAngles_CosYaw) + flDeltaX * flMainViewAngles_SinYaw;
	float y = flDeltaX * (-flMainViewAngles_CosYaw) - flDeltaY * flMainViewAngles_SinYaw;

	float flRange = 2000;

	if (fabs(x) > flRange || fabs(y) > flRange)
	{
		// clipping
		if (y > x)
		{
			if (y > -x)
			{
				x = flRange * x / y;
				y = flRange;
			}
			else
			{
				y = -flRange * y / x;
				x = -flRange;
			}
		}
		else
		{
			if (y > -x)
			{
				y = flRange * y / x;
				x = flRange;
			}
			else
			{
				x = -flRange * x / y;
				y = -flRange;
			}
		}
	}

	int	iScreenX = gPlayerVars.iRadarPosX + int(x / flRange * gPlayerVars.iRadarSize);
	int iScreenY = gPlayerVars.iRadarPosY + int(y / flRange * gPlayerVars.iRadarSize);

	gDrawManager.DrawRect(iScreenX - 3, iScreenY - 3, 7, 7, COLORCODE(0, 0, 0, 255));
	gDrawManager.DrawRect(iScreenX - 2, iScreenY - 2, 5, 5, dwTeamColor);

}
//===================================================================================
void CRadar::DrawRadarBack()
{
	GetCursorPos(&Mouse);

	int iSize = gPlayerVars.iRadarSize;
	int iCenterX = gPlayerVars.iRadarPosX;
	int iCenterY = gPlayerVars.iRadarPosY;

	if (gMenu.MouseInRegion(iCenterX - iSize, iCenterY - iSize, iSize * 2, iSize * 2) && GetAsyncKeyState(VK_LBUTTON))
	{
		if (!bRadarPos)
		{
			SavedRadarX = Mouse.x - gPlayerVars.iRadarPosX;
			SavedRadarY = Mouse.y - gPlayerVars.iRadarPosY;
			bRadarPos = true;
		}

		gPlayerVars.iRadarPosX = Mouse.x;
		gPlayerVars.iRadarPosY = Mouse.y;

		gPlayerVars.iRadarPosX -= SavedRadarX;
		gPlayerVars.iRadarPosY -= SavedRadarY;
	}
	else
	{
		bRadarPos = false;
	}

	if (gPlayerVars.bRadar)
	{
		gDrawManager.OutlineRect(iCenterX - iSize - 7, iCenterY - iSize - 17, 2 * iSize + 16, 2 * iSize + 25, COLORCODE(0, 0, 0, 150));
		gDrawManager.DrawRect(iCenterX - iSize - 6, iCenterY - iSize - 16, 2 * iSize + 14, 2 * iSize + 23, COLORCODE(255, 120, 25, 250));
		gDrawManager.DrawString(gDrawManager.GetFont(), iCenterX - iSize, iCenterY - iSize - 16, COLOR_WHITE, "Radar");
		gDrawManager.DrawRect(iCenterX - iSize, iCenterY - iSize, 2 * iSize + 2, 2 * iSize + 2, COLORCODE(30, 30, 30, 255));
		gDrawManager.DrawRect(iCenterX, iCenterY - iSize, 1, 2 * iSize + 2, COLORCODE(255, 120, 25, 250));
		gDrawManager.DrawRect(iCenterX - iSize, iCenterY, 2 * iSize + 2, 1, COLORCODE(255, 120, 25, 250));
	}
}

void CRadar::Think(CBaseEntity* pLocal)
{
	if (!gPlayerVars.bRadar)
		return;

	//Draw the radar before drawing the player.
	DrawRadarBack();

	for (int i = 1; i <= gInts.Engine->GetMaxClients(); i++)
	{
		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity)
			continue;

		if (!pEntity->IsAlive() || pEntity->IsDormant())
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam() && !gPlayerVars.bTeammates)
			continue;

		static DWORD dwColor;

		switch (pEntity->GetTeam())
		{
		case AXIS:
			dwColor = COLOR_DOD_GREEN; break;
		case WEHRMACHT:
			dwColor = COLOR_DOD_RED; break;
		}

		DrawRadarPoint(pEntity->GetAbsOrigin(), pLocal->GetAbsOrigin(), pLocal->GetAbsAngles(), pEntity, dwColor);
	}
}
