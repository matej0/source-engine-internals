#include "SDK.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Client.h"
#include "CMenu.h"


#define DISPLAY_DEBUG_INFO 0
void Debug(CBaseEntity* pLocal)
{
	if (!pLocal || !pLocal->IsAlive())
		return;

	if (!pLocal->GetActiveWeapon())
		return;

	auto pWpnData = pLocal->GetActiveWeapon()->GetCSWpnData();
	
	if (pWpnData)
		gDrawManager.DrawString(gDrawManager.GetFont(), 500, 200, COLOR_WHITE, "success!");

	gDrawManager.DrawString(gDrawManager.GetFont(), 500, 214, COLOR_WHITE, "name %s", pWpnData->szWeaponName);
	//auto sequence = gVars.DEBUG_nSequence;
	//gDrawManager.DrawString(gDrawManager.GetFont(), 500, 200, COLOR_WHITE, "next shot ticks: %d", gVars.);
	//gDrawManager.DrawString(gDrawManager.GetFont(), 500, 220, COLOR_WHITE, "next shot time: %.2f", gVars.DEBUG_flTimeUntilNextShot);
	///gDrawManager.DrawString(gDrawManager.GetFont(), 500, 240, COLOR_WHITE, "next attack: %.2f", gVars.DEBUG_flNextAttack);
	//gDrawManager.DrawString(gDrawManager.GetFont(), 500, 200, COLOR_WHITE, "points: %d", gVars.DEBUG.nAimbotPoints);
	
}

//unused but should work, whatevz.
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

	if (!pEntity->SetupBones(matrix, 128, 0x0007FF00, gInts.Globals->curtime))
		return;

	for (int i{ }; i < pHdr->numbones; ++i)
	{
		pStudioBone = pHdr->GetBone(i);

		if (!pStudioBone || !(pStudioBone->flags & 256))
			continue;

		parent = pStudioBone->parent;
		if (parent == -1)
			continue;

		vBonePos = { matrix[i][0][3], matrix[i][1][3], matrix[i][2][3] };
		vParentPos = { matrix[parent][0][3], matrix[parent][1][3], matrix[parent][2][3] };

		if (gDrawManager.WorldToScreen(vBonePos, vBonePosScreen) && gDrawManager.WorldToScreen(vParentPos, vParentPosScreen))
			gDrawManager.DrawLine(vBonePosScreen.x, vBonePosScreen.y, vParentPosScreen.x, vParentPosScreen.y, COLORCODE(255, 255, 255, 200));
	}
}

void Player_ESP(CBaseEntity* pLocal)
{
	if (!gVars.bESP)
		return;

	for (int i = 1; i <= gInts.Globals->maxClients; i++) 
	{
		CBaseEntity* pEntity = GetBaseEntity(i);

		if (!pEntity)
			continue;

		if (pEntity == pLocal)
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam())
			continue;

		if (!pEntity->IsAlive() || pEntity->IsDormant())
			continue;

		player_info_t info;

		if (!gInts.Engine->GetPlayerInfo(i, &info))
			continue;

		int x, y, w, h;
		if (!gDrawManager.CalculatePlayerBox(pEntity, x, y, w, h))
			continue;

		int Y = y;
		int iMaxHP = 100;
		int iHP = (std::min)(100, pEntity->GetHealth());
		int iGreen = (255 * pEntity->GetHealth()) / iMaxHP;
		int iRed = 255 - iGreen;

		int iFill = (int)std::round(h * iHP / iMaxHP);

		DWORD dwColor = COLORCODE(iRed, iGreen, 0, 255);
		
		if (gVars.bBox)
		{
			gDrawManager.OutlineRect(x - 1, y - 1, w + 2, h + 2, COLORCODE(0, 0, 0, 255));
			gDrawManager.OutlineRect(x, y, w, h, dwColor);
			gDrawManager.OutlineRect(x + 1, y + 1, w - 2, h - 2, COLORCODE(0, 0, 0, 255));
		}

		if (gVars.bHealthBar)
		{
			gDrawManager.DrawRect(x - 8, y - 1, 5, h + 2, COLORCODE(25, 25, 25, 230));
			gDrawManager.DrawRect(x - 7, y + h - iFill, 3, iFill, COLORCODE(iRed, iGreen, 0, 255));
		}

		if (gVars.bShowInfo)
		{
			wchar_t szString[1024];
			memset(szString, 0, sizeof(wchar_t) * 1024);

			if (MultiByteToWideChar(CP_UTF8, 0, info.name, MAX_PLAYER_NAME_LENGTH, szString, 1024))
				gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 5, Y, COLOR_WHITE, szString);
			else
				gDrawManager.DrawString(gDrawManager.GetAltFont(), x + w + 5, Y, COLOR_WHITE, "%s", info.name);

			Y += gDrawManager.GetESPHeight();

			gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, x + w + 5, Y, COLOR_WHITE, gDrawManager.GetCurrentWeapon(pEntity));

		}

		if (gVars.bSkeleton)
			Skeleton(pEntity);
	}
}

void OnPaintTraverse()
{
	int iHeight, iWidth;
	gInts.Engine->GetScreenSize(iWidth, iHeight);

	time_t t = std::time(nullptr);
	std::ostringstream time;

	time << std::put_time(std::localtime(&t), ("%T"));
	gDrawManager.DrawString(gDrawManager.GetFont(), iHeight -= iHeight - 5, iWidth -= iWidth - 5, COLORCODE(255, 255, 255, 255), "%s", time.str().data());

	if (gInts.Engine->IsDrawingLoadingImage() || !gInts.Engine->IsInGame() || !gInts.Engine->IsConnected() || ((GetAsyncKeyState(VK_F12) || gInts.Engine->IsTakingScreenshot())))
	{
		return;
	}

	CBaseEntity* pBaseLocalEnt = gInts.EntList->GetClientEntity(me);

	if (pBaseLocalEnt == NULL)
		return;

#if DISPLAY_DEBUG_INFO == 1
	Debug(pBaseLocalEnt);
#endif

	Player_ESP(pBaseLocalEnt);
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

		OnPaintTraverse();

		gCheatMenu.HandleMenuInput();

		if (gCheatMenu.bMenuActive)
		{
			gCheatMenu.DrawMenu();
			gCheatMenu.Render();
		}
	}
}

void SetMaterialProperties(IMaterial* pModel, float r, float b, float g, bool ignorez, float alpha = 1.f)
{
	pModel->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez);
	pModel->SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, true);
	pModel->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);

	float fColor[4] = { r / 255, g / 255, b / 255, alpha };

	static float fGetColor[4];

	gInts.RenderView->GetColorModulation(fGetColor);

	if (fGetColor[0] != fColor[0] || fGetColor[1] != fColor[1] || fGetColor[2] != fColor[2])
	{
		gInts.RenderView->SetColorModulation(fColor);
	}

	gInts.RenderView->SetBlend(alpha);

	gInts.ModelRender->ForcedMaterialOverride(pModel);
}


void __fastcall Hooked_DrawModelExecute(PVOID thisptr, PVOID edx, PVOID ctx, PVOID state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	static IMaterial* pModel = gInts.MaterialSystem->FindMaterial("debug/debugambientcube", "Model textures");
	static IMaterial* pBacktrackModel = gInts.MaterialSystem->FindMaterial("debug/debugambientcube", "Model textures");

	static DWORD dwColorVisible, dwColorIgnoreZ;
	const char* szModelName = gInts.ModelInfo->GetModelName((DWORD*)pInfo.pModel);
	CBaseEntity* pModelEntity = gInts.EntList->GetClientEntity(pInfo.entity_index);
	CBaseEntity* pLocal = gInts.EntList->GetClientEntity(me);

	if (gVars.bChams)
	{
		if (szModelName && strstr(szModelName, "models/player"))
		{
			if (pModelEntity && pLocal)
			{
				switch (pModelEntity->GetTeam())
				{
				case AXIS:
					dwColorVisible = COLORCODE(255, 255, 0, 255);
					dwColorIgnoreZ = COLORCODE(255, 0, 0, 255);
					break;
				case WEHRMACHT:
					dwColorVisible = COLORCODE(0, 255, 0, 255);
					dwColorIgnoreZ = COLORCODE(0, 0, 255, 255);
					break;
				}

				if (pModelEntity->GetTeam() != pLocal->GetTeam())
				{
					if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0)
					{
						
						/*if (gVars.bLagChams && gVars.bBacktracking)
						{
							for (int i = 0; i < gVars.nBacktrackTicks; i++)
							{
								DWORD dwLagChams = COLOR_WHITE;
								SetMaterialProperties(pBacktrackModel, RED(dwLagChams), BLUE(dwLagChams), GREEN(dwLagChams), false, 0.2f);
								oDrawModel(thisptr, ctx, state, pInfo, Data[pInfo.entity_index][i].m_Matrix);
							}
						}*/

						SetMaterialProperties(pModel, RED(dwColorVisible), BLUE(dwColorVisible), GREEN(dwColorVisible), false);
						oDrawModel(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						

						if (gVars.bXQZChams)
						{
							SetMaterialProperties(pModel, RED(dwColorIgnoreZ), BLUE(dwColorIgnoreZ), GREEN(dwColorIgnoreZ), true);
							oDrawModel(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}
					}
				}
			}
		}
	}
	
	oDrawModel(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	gInts.ModelRender->ForcedMaterialOverride(NULL);
}

void Intro(void)
{
	gDrawManager.Initialize(); //Initalize the drawing class.
}

