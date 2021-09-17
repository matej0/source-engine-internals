#include "SDK.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Client.h"
#include "CMenu.h"
//55 8B EC 81 EC ? ? ? ? 53 8B 5D 08 56 57 6A KeyValues::GetString

CScreenSize gScreenSize;
//==================================================================================

void Debug(CBaseEntity* pLocal)
{
	if (!pLocal || !pLocal->IsAlive())
		return;

	if (!pLocal->GetActiveWeapon())
		return;

	//gDrawManager.DrawString(gDrawManager.GetAltFont(), 200, 100, COLORCODE(255, 255, 255, 255), "SIDE: %d", gPlayerVars.iAASide);


	DWORD *pDODWpnData = pLocal->GetActiveWeapon()->GetDODWpnData();

	if (!pDODWpnData)
		return;

	int   m_WeaponType             = *(int*)(pDODWpnData + 0x1CE); //462
	int	  m_iDamage                = *(int*)(pDODWpnData + 0x1BE); //446
	float m_flAccuracy             = *(float*)(pDODWpnData + 0x1C7); //455
	float m_flSecondaryAccuracy    = *(float*)(pDODWpnData + 0x1C8); //456
	float m_flAccuracyMovePenalty  = *(float*)(pDODWpnData + 0x1C9); //457
	float m_flPenetration          = *(float*)(pDODWpnData + 0x1BF); //447
	
	gDrawManager.DrawString(gDrawManager.GetAltFont(), 200, 100, COLORCODE(255, 255, 255, 255), "Accuracy: %.2f", m_flAccuracy);
	gDrawManager.DrawString(gDrawManager.GetAltFont(), 200, 114, COLORCODE(255, 255, 255, 255), "AccuracyMovePenalty: %.2f", m_flAccuracyMovePenalty);
	gDrawManager.DrawString(gDrawManager.GetAltFont(), 200, 128, COLORCODE(255, 255, 255, 255), "SecondaryAccuracy: %.2f", m_flSecondaryAccuracy);
	gDrawManager.DrawString(gDrawManager.GetAltFont(), 200, 142, COLORCODE(255, 255, 255, 255), "Penetration: %.2f", m_flPenetration);
	gDrawManager.DrawString(gDrawManager.GetAltFont(), 200, 156, COLORCODE(255, 255, 255, 255), "Damage: %d", m_iDamage);
}


void Miscellaneous(CBaseEntity* pLocal)
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


void Indicator(CBaseEntity* pEntity, CBaseEntity* pLocal, DWORD dwClr)
{
	Vector vS;

	if (!gDrawManager.WorldToScreen(pEntity->GetAbsOrigin(), vS))
		return;

	if (streql(pEntity->GetClientClass()->chName, "CDODBaseGrenade") || streql(pEntity->GetClientClass()->chName, "CDODRifleGrenadeGER") || streql(pEntity->GetClientClass()->chName, "CDODRifleGrenadeUS"))
	{
		gDrawManager.DrawString(gDrawManager.GetAltFont(), vS.x, vS.y, dwClr, "Grenade");
	}
	else if (streql(pEntity->GetClientClass()->chName, "CDODBaseRocket"))
	{
		gDrawManager.DrawString(gDrawManager.GetAltFont(), vS.x, vS.y, dwClr, "Rocket");
	}
}

void WaterMark()
{
	time_t t = std::time(nullptr);
	std::ostringstream time;
	time << std::put_time(std::localtime(&t), ("%T, %a %b %d %Y"));

	int m_width, m_height;
	gInts.Engine->GetScreenSize(m_width, m_height);

	Vector vP = (Vector(20, 150, 0));
	int h = 35;
	int y = vP.y + 2;

	//nitro(?)
	//FF6243

	//shapes
	gDrawManager.DrawRect(vP.x, vP.y - 2, 260, 2, COLORCODE(255, 120, 25, 250));
	gDrawManager.DrawRect(vP.x, vP.y, 260, h, COLORCODE(45, 45, 45, 200));

	//strings
	gDrawManager.DrawString(gDrawManager.GetFont(), vP.x + 3, y, COLORCODE(255, 255, 255, 255), "reDodstorm, %s", time.str().data());
	y += gDrawManager.GetMenuHeight();

	//i wanted to use std::string_view however i would need to switch to c++17 
	//and im not sure if i could do that because iirc some stuff here is deprecated/removed.
	std::string strFps = "FPS: " + std::to_string(gDrawManager.GetFPS());
	std::string strPing = "PING: " + std::to_string(gDrawManager.GetPing());
	std::string strTickrate = "TICK: " + std::to_string(gDrawManager.GetTickRate());
	std::string flLerp = "LERP: " + std::to_string(gDrawManager.GetClientInterpAmount());
	int iAdjust = vP.x + 3;

	gDrawManager.DrawString(gDrawManager.GetFont(), iAdjust, y, COLORCODE(255, 120, 25, 255), strFps.c_str());
	iAdjust += gDrawManager.GetWidth(gDrawManager.GetFont(), strFps.c_str()) + 8;
	gDrawManager.DrawString(gDrawManager.GetFont(), iAdjust, y, COLORCODE(255, 120, 25, 255), strPing.c_str());
	iAdjust += gDrawManager.GetWidth(gDrawManager.GetFont(), strPing.c_str()) + 8;
	gDrawManager.DrawString(gDrawManager.GetFont(), iAdjust, y, COLORCODE(255, 120, 25, 255), strTickrate.c_str());
	iAdjust += gDrawManager.GetWidth(gDrawManager.GetFont(), strTickrate.c_str()) + 8;
	gDrawManager.DrawString(gDrawManager.GetFont(), iAdjust, y, COLORCODE(255, 120, 25, 255), flLerp.c_str());
	y += gDrawManager.GetMenuHeight();
}

void Skeleton(CBaseEntity* pEntity)
{
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
	if (!pEntity->SetupBones(matrix, 128, 0x0007FF00, gInts.Globals->curtime))
		return;

	for (int i{ }; i < pHdr->numbones; ++i)
	{
		pStudioBone = pHdr->GetBone(i);
		if (!pStudioBone || !(pStudioBone->flags & 256))
			continue;

		//get parent bone.
		parent = pStudioBone->parent;
		if (parent == -1)
			continue;

		vBonePos = { matrix[i][0][3], matrix[i][1][3], matrix[i][2][3] };
		vParentPos = { matrix[parent][0][3], matrix[parent][1][3], matrix[parent][2][3] };

		if (gDrawManager.WorldToScreen(vBonePos, vBonePosScreen) && gDrawManager.WorldToScreen(vParentPos, vParentPosScreen))
			gDrawManager.DrawLine(vBonePosScreen.x, vBonePosScreen.y, vParentPosScreen.x, vParentPosScreen.y, COLORCODE(255, 255, 255, 200));
	}
}

void BacktrackSkeleton(CBaseEntity* pEntity, int tick)
{
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

	for (int i{ }; i < pHdr->numbones; ++i)
	{
		pStudioBone = pHdr->GetBone(i);
		if (!pStudioBone || !(pStudioBone->flags & 256))
			continue;

		//get parent bone.
		parent = pStudioBone->parent;
		if (parent == -1)
			continue;

		vBonePos = { LagCompensationData[pEntity->GetIndex()][tick].Matrix[i][0][3], LagCompensationData[pEntity->GetIndex()][tick].Matrix[i][1][3], LagCompensationData[pEntity->GetIndex()][tick].Matrix[i][2][3] };
		vParentPos = { LagCompensationData[pEntity->GetIndex()][tick].Matrix[parent][0][3], LagCompensationData[pEntity->GetIndex()][tick].Matrix[parent][1][3], LagCompensationData[pEntity->GetIndex()][tick].Matrix[parent][2][3] };

		if (gDrawManager.WorldToScreen(vBonePos, vBonePosScreen) && gDrawManager.WorldToScreen(vParentPos, vParentPosScreen))
			gDrawManager.DrawLine(vBonePosScreen.x, vBonePosScreen.y, vParentPosScreen.x, vParentPosScreen.y, COLORCODE(255, 255, 255, 200));
	}
}

void Wallhack(CBaseEntity* pLocal)
{
	for (int i = 1; i <= gInts.EntList->GetHighestEntityIndex(); i++)
	{
		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity || pEntity->IsDormant() || pEntity == pLocal)
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam() && !gPlayerVars.bTeammates)
			continue;

		player_info_t info;

		Vector vecWorld, vecScreen;
		pEntity->GetWorldSpaceCenter(vecWorld);
		if (!gDrawManager.WorldToScreen(vecWorld, vecScreen))
			continue;

		static DWORD dColor;
		int x, y, w, h;
		gDrawManager.CalculateBounds(pEntity, x, y, w, h);

		int xr = x + w + 3;
		int yr = y;

		switch (pEntity->GetTeam())
		{
		case AXIS:
			dColor = COLOR_DOD_GREEN; break;
		case WEHRMACHT:
			dColor = COLOR_DOD_RED; break;
		}

		if (gInts.Engine->GetPlayerInfo(i, &info) && pEntity->IsAlive())
		{
			if (gPlayerVars.bBox)
			{
				gDrawManager.OutlineRect(x - 1, y - 1, w + 2, h + 2, COLORCODE(0, 0, 0, 255));
				gDrawManager.OutlineRect(x, y, w, h, dColor);
				gDrawManager.OutlineRect(x + 1, y + 1, w - 2, h - 2, COLORCODE(0, 0, 0, 255));
			}

			if (gPlayerVars.bName)
			{
				wchar_t szString[1024];
				memset(szString, 0, sizeof(wchar_t) * 1024);

				if (MultiByteToWideChar(CP_UTF8, 0, info.name, MAX_PLAYER_NAME_LENGTH, szString, 1024))
				{
					gDrawManager.DrawString(gDrawManager.GetAltFont(), xr, yr, COLOR_WHITE, szString);
					yr += gDrawManager.GetESPHeight();
				}
				else
				{
					gDrawManager.DrawString(gDrawManager.GetAltFont(), xr, yr, COLOR_WHITE, "%s", info.name);
					yr += gDrawManager.GetESPHeight();
				}
			}
			
			if (gPlayerVars.bHealth)
			{
				int iHP = pEntity->GetHealth();
				int iGreen = (255 * pEntity->GetHealth()) / 100;
				int iRed = 255 - iGreen;

				gDrawManager.DrawRect(x - 8, y - 1, 5, h + 2, COLORCODE(25, 25, 25, 180));
				gDrawManager.DrawRect(x - 7, y + (h - (h * iHP / 100)), 3, h * iHP / 100, COLORCODE(iRed, iGreen, 0, 240));

				if (iHP < 100)
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), true, x - 5, y + (h - (h * iHP / 100)) - 5, COLOR_WHITE, "%d", iHP);
			}
			
			switch (gPlayerVars.iTest)
			{
			case 1:
				gDrawManager.DrawString(gDrawManager.GetAltFont(), xr, yr, COLOR_WHITE, gDrawManager.GetWeaponType(pEntity)); break;
			case 2:
				gDrawManager.DrawString(gDrawManager.GetAltFont(), xr, yr, COLOR_WHITE, gDrawManager.szCurrentWeaponName(pEntity)); break;
			}

			if (gPlayerVars.bWeapon)
			{
			}

			if (gPlayerVars.bSkeleton)
			{
				Skeleton(pEntity);

				//if (pLocal->IsAlive())
				//{
				//	BacktrackSkeleton(pEntity, 1);
				//}
			}
		}

		Indicator(pEntity, pLocal, dColor);
	}
}

void OnPaintTraverse()
{
	if (gInts.Engine->IsDrawingLoadingImage() || !gInts.Engine->IsInGame() || !gInts.Engine->IsConnected() || ((GetAsyncKeyState(VK_F12) || gInts.Engine->IsTakingScreenshot())))
	{
		return; 
	}

	CBaseEntity* pBaseLocalEnt = gInts.EntList->GetClientEntity(me);

	if (pBaseLocalEnt == NULL) 
		return;


	//WaterMark();
	Wallhack(pBaseLocalEnt);
	Miscellaneous(pBaseLocalEnt);
	Debug(pBaseLocalEnt);
}

void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce )
{
	
	oPanel(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.

	static unsigned int vguiFocusOverlayPanel;

	if (vguiFocusOverlayPanel == NULL)
	{
		const char* szName = gInts.Panels->GetName(vguiPanel);
		if (szName[0] == 'F' && szName[5] == 'O' &&  szName[12] == 'P')
		{
			vguiFocusOverlayPanel = vguiPanel;
			Intro();
		}
	}

	if (vguiFocusOverlayPanel == vguiPanel)
	{
		//prevent double draw.
		//seems to only be an issue with FocusOverlayPanel
		static bool bSkip = false;
		bSkip = !bSkip;

		if (bSkip)
			return;
	
		if (GetAsyncKeyState(VK_INSERT) & 1)
			gMenu.IsMenuOpen = !gMenu.IsMenuOpen;

		if (gMenu.IsMenuOpen)
			gMenu.Draw();

		gInts.Panels->SetMouseInputEnabled(vguiPanel, gMenu.IsMenuOpen);
		OnPaintTraverse();
	}
}
//===================================================================================
void Intro(void)
{
	gDrawManager.Initialize(); //Initalize the drawing class.
	gMenu.Initialize(); //Initialize the menu.
}

void SetMaterialProperties(IMaterial* pModel, float r, float b, float g, bool ignorez, float alpha = 1.f)
{
	pModel->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez);
	pModel->SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, true);
	pModel->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);

	float fColor[4] = { r / 255, g / 255, b / 255, 1.0f };

	static float fGetColor[4];

	gInts.RenderView->GetColorModulation(fGetColor);

	if (fGetColor[0] != fColor[0] || fGetColor[1] != fColor[1] || fGetColor[2] != fColor[2])
		gInts.RenderView->SetColorModulation(fColor);

	gInts.RenderView->SetBlend(alpha);

	gInts.ModelRender->ForcedMaterialOverride(pModel);
}

void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	//static IMaterial* pPlayerAxis = gInts.MaterialSystem->FindMaterial("models/player/american/american_body", "Model textures");
	//static IMaterial* pPlayerWehrmacht = gInts.MaterialSystem->FindMaterial("models/player/german/german_body", "Model textures");

	//pPlayerAxis->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);
	//pPlayerWehrmacht->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);

	static IMaterial* pModel = gInts.MaterialSystem->FindMaterial("debug/debugambientcube", "Model textures");

	//static IMaterial* pModel = gInts.MaterialSystem->FindMaterial("glowvisible", "Model textures");
	//pModel->IncrementReferenceCount();

	static DWORD dwColVisible, dwColXQZ;
	const char* szModelName = gInts.ModelInfo->GetModelName((DWORD*)pInfo.pModel);
	CBaseEntity* pModelEntity = gInts.EntList->GetClientEntity(pInfo.entity_index);
	CBaseEntity* pLocal = gInts.EntList->GetClientEntity(me);

	if (gPlayerVars.bChams)
	{
		if (szModelName && strstr(szModelName, "models/player"))
		{
			if (pModelEntity && pLocal)
			{
				if (pModelEntity->GetTeam() != pLocal->GetTeam())
				{
					if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0)
					{
						switch (pModelEntity->GetTeam())
						{
						case AXIS:
							dwColVisible = COLOR_DOD_GREEN; 
							dwColXQZ = COLORCODE(0, 0, 255, 255);
							break;
						case WEHRMACHT:
							dwColVisible = COLOR_DOD_RED;
							dwColXQZ = COLORCODE(255, 255, 0, 255);
							break;
						}

						if (gPlayerVars.bXQZ)
						{
							SetMaterialProperties(pModel, RED(dwColXQZ), BLUE(dwColXQZ), GREEN(dwColXQZ), true);
							oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
						}

						SetMaterialProperties(pModel, RED(dwColVisible), BLUE(dwColVisible), GREEN(dwColVisible), false);
						oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
					}
				}
			}
		}
	}
	
	oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
	gInts.ModelRender->ForcedMaterialOverride(NULL);
}
