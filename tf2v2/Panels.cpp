#include "Panels.h"
#include "CDrawManager.h"
#include "Client.h"
#include "CMenu.h"
#include "ESP.h"
#include "Aimbot.h"
CScreenSize gScreenSize;
bool bMenu;

#define DISPLAY_DEBUG_VALUES 0
void Debug(CBaseEntity* pLocal)
{
	if (!pLocal || !pLocal->IsAlive())
		return;

	if (!pLocal->GetActiveWeapon())
		return;


	gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, 50, 200, COLOR_WHITE, "knife: %s", gPlayerVars.Debug.m_bHoldingKnife ? "true" : "false");
	gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, 50, 220, COLOR_WHITE, "in range: %s", gPlayerVars.Debug.m_bInBackstabRange ? "true" : "false");
	gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, 50, 240, COLOR_WHITE, "behind: %s", gPlayerVars.Debug.m_bIsBehindTarget ? "true" : "false");
}


void OnPaintTraverse()
{
	if (gInts.Engine->IsDrawingLoadingImage() || !gInts.Engine->IsInGame() || !gInts.Engine->IsConnected() || ((GetAsyncKeyState(VK_F12) || gInts.Engine->IsTakingScreenshot())))
		return; 

	CBaseEntity* pBaseLocalEnt = gInts.EntList->GetClientEntity(me);  

	if (pBaseLocalEnt == NULL) 
		return;

	gRadar.Think(pBaseLocalEnt);
	gESP.SpectatorsList(pBaseLocalEnt);
	gESP.Think(pBaseLocalEnt);
	//gESP.Players(pBaseLocalEnt);
	//gESP.Buildings(pBaseLocalEnt);
	gESP.FakeLagIndicator(pBaseLocalEnt);

#if DISPLAY_DEBUG_VALUES == 1
	Debug(pBaseLocalEnt);
#endif
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


		gInts.Panels->SetMouseInputEnabled(vguiPanel, gMenu.m_bIsOpen);
	}
}
//===================================================================================
void Intro(void)
{
	gDrawManager.Initialize(); //Initalize the drawing class.
}

void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	static IMaterial* pModel = gInts.MaterialSystem->FindMaterial("debug/debugambientcube", "Model textures");
	static IMaterial* pBacktrackModel = gInts.MaterialSystem->FindMaterial("debug/debugambientcube", "Model textures");

	pModel->SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, true);

	const char* szModelName = gInts.ModelInfo->GetModelName((DWORD*)pInfo.pModel);
	CBaseEntity* pModelEntity = gInts.EntList->GetClientEntity(pInfo.entity_index);
	CBaseEntity* pLocal = gInts.EntList->GetClientEntity(me);
	
	if (gPlayerVars.bEspPlayers)
	{
		if (szModelName && (strstr(szModelName, "models/player")))
		{
			if (pModelEntity && pLocal)
			{
				
				if (pModelEntity->GetTeam() != pLocal->GetTeam())
				{
					if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0)
					{
						if (gPlayerVars.bBacktrackChams && gPlayerVars.bBacktracking && !gAim.m_Records[pInfo.entity_index].empty())
						{
							//calling isvalid in here? idk maybe i should just remove records in fsn instead of checking like this whatever
							ILagData* pRecord = gAim.GetLastRecord(pInfo.entity_index);
							bool bIsValid = !pRecord->IsRecordOutOfBounds();

							if (bIsValid)
							{
								static float col2[3] = { 1.f, 1.f, 1.f };
								pModel->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
								gInts.RenderView->SetColorModulation(col2);
								gInts.RenderView->SetBlend(0.5f);
								gInts.ModelRender->ForcedMaterialOverride(pModel);
								oDrawModel(gInts.ModelRender, state, pInfo, pRecord->m_pMatrix);

								//reset blend and set material to null. this makes it so that if you use only backtrack chams, it wont color the model white.
								gInts.RenderView->SetBlend(1.f);
								gInts.ModelRender->ForcedMaterialOverride(NULL);
								oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld); //reset to normal matrix. this stops the model from flickering.

							}
						}

						if (gPlayerVars.bXQZ)
						{
							pModel->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							float col[3] = { gPlayerVars.clrXQZChams[0], gPlayerVars.clrXQZChams[1], gPlayerVars.clrXQZChams[2] };
							gInts.RenderView->SetColorModulation(col);
							gInts.RenderView->SetBlend(1.0f);
							gInts.ModelRender->ForcedMaterialOverride(pModel);

							oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
						}
						if (gPlayerVars.bChams)
						{
							pModel->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
							float col3[3] = { gPlayerVars.clrVisibleChams[0], gPlayerVars.clrVisibleChams[1], gPlayerVars.clrVisibleChams[2] };
							gInts.RenderView->SetColorModulation(col3);
							gInts.RenderView->SetBlend(1.0f);
							gInts.ModelRender->ForcedMaterialOverride(pModel);
							oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
						}
						
					}
				}
			}
		}

		if (szModelName && strstr(szModelName, "weapon") && pModelEntity)
		{
			CBaseEntity* pOwner = gInts.EntList->GetClientEntityFromHandle(pModelEntity->GetOwner());

			if (pOwner && pOwner != pLocal)
			{
				if (pModelEntity->GetTeam() != pLocal->GetTeam())
				{
					if (gPlayerVars.bXQZ)
					{
						pModel->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						float col[3] = { gPlayerVars.clrXQZChams[0], gPlayerVars.clrXQZChams[1], gPlayerVars.clrXQZChams[2] };
						gInts.RenderView->SetColorModulation(col);
						gInts.RenderView->SetBlend(1.0f);
						gInts.ModelRender->ForcedMaterialOverride(pModel);

						oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
					}

					
					if (gPlayerVars.bChams)
					{
						pModel->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						float col3[3] = { gPlayerVars.clrVisibleChams[0], gPlayerVars.clrVisibleChams[1], gPlayerVars.clrVisibleChams[2] };
						gInts.RenderView->SetColorModulation(col3);
						gInts.RenderView->SetBlend(1.0f);
						gInts.ModelRender->ForcedMaterialOverride(pModel);
						oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
					}

				}
			}
		}
	}
	
	oDrawModel(gInts.ModelRender, state, pInfo, pCustomBoneToWorld);
	gInts.ModelRender->ForcedMaterialOverride(NULL);
}

static bool once = false;
static bool twice = false;
HRESULT STDMETHODCALLTYPE Hooked_EndScene(IDirect3DDevice9* pDevice)
{
	//init imgui related shit that only needs to be called once.
	if (!once)
	{
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(gPlayerVars.hWindow);
		ImGui_ImplDX9_Init(pDevice);
		ImGui_ImplDX9_CreateDeviceObjects();
		ImGui::DarkModernStyle();
		once = true;
	}

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX9_NewFrame();

	ImGui::NewFrame();

	if (gMenu.m_bIsOpen)
	{
		//ImGui::ShowStyleEditor(); 
		//ImGui::ShowDemoWindow();

		gMenu.Draw();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return oEndScene(pDevice);
}

LRESULT STDMETHODCALLTYPE Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		gMenu.m_bIsOpen = !gMenu.m_bIsOpen;

	if (gMenu.m_bIsOpen && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return TRUE;

	return CallWindowProc(gPlayerVars.pWndProc, hWnd, uMsg, wParam, lParam);
}