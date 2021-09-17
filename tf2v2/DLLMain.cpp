#include "SDK.h"
#include "Client.h"
#include "Panels.h"
#include "Config.h"
//doubt this will ever see the light of day ( unknowncheats ).


COffsets                                gOffsets;
CPlayerVariables                        gPlayerVars;
CInterfaces                             gInts;
CFriends                                gFriends;
CConvars                                gConvars;
CMath                                   gMath;
CDebug                                  gDebug;

CreateMoveFn bReturn                    = nullptr;
PanelsFn oPanel                         = nullptr;
FrameStageFn oFrameStage                = nullptr;
OverrideViewFn oView                    = nullptr;
RunCommandFn oRunCommand                = nullptr;
DrawModelExecuteFn oDrawModel           = nullptr;
ShouldDrawFogFn oFog                    = nullptr;
IsHltvFn oIsHltv                        = nullptr;
LevelInitPreEntity oLevelInit           = nullptr;
LevelInitPostEntity oLevelPost          = nullptr;
IsPlayingBackFn oIsPlayingBack          = nullptr;
IsPlayingTimeDemoFn oIsPlayingTimeDemo  = nullptr;
OuterAbsVelocityFn oGetOuterAbsVelocity = nullptr;
EstimateAbsVelocityFn oEstimateAbsVelocity = nullptr;
UpdatePlayerListFn oUpdatePlayerList    = nullptr;
TeleportedFn oTeleported = nullptr;
AddFlagFn oAddFlag = nullptr;
UpdateClientSideAnimationsFn oUpdateClientSideAnimations = nullptr;
ItemPostFrameFn oItemPostFrame = nullptr;

EndSceneFn oEndScene                    = nullptr;

CreateInterface_t EngineFactory         = NULL;
CreateInterface_t ClientFactory         = NULL;
CreateInterface_t VGUIFactory           = NULL;
CreateInterface_t VGUI2Factory          = NULL;
CreateInterfaceFn MaterialSystemFactory = NULL;
CreateInterface_t CvarFactory           = NULL;

IDirect3DDevice9* pDevice = NULL;

DWORD WINAPI dwMainThread(HMODULE hInstance, LPVOID lpArguments )
{

	if (gInts.Client == NULL)
	{
		ClientFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "client.dll" ), "CreateInterface" );
		EngineFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface");
		VGUIFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vguimatsurface.dll"), "CreateInterface");
		VGUI2Factory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vgui2.dll"), "CreateInterface");
		MaterialSystemFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("MaterialSystem.dll"), "CreateInterface");
		CvarFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vstdlib.dll"), "CreateInterface");

		gInts.Client         = ( CHLClient* )ClientFactory( "VClient017", NULL);
		gInts.EntList        = ( CEntList* ) ClientFactory( "VClientEntityList003", NULL );
		gInts.Engine         = ( EngineClient* ) EngineFactory( "VEngineClient013", NULL );
		gInts.Surface        = ( ISurface* ) VGUIFactory( "VGUI_Surface030", NULL );
		gInts.Prediction     = (CPrediction *)(ClientFactory("VClientPrediction001", NULL));
		gInts.Panels         = (IPanel*)VGUI2Factory("VGUI_Panel009", NULL);
		gInts.DebugOverlay   = (CDebugOverlay*)(EngineFactory("VDebugOverlay003", NULL));
		gInts.Globals        = *reinterpret_cast<CGlobals**>(gSignatures.GetEngineSignature("A1 ? ? ? ? 8B 11 68") + 8);
		gInts.ModelInfo      = (IVModelInfo*)EngineFactory("VModelInfoClient006", NULL);
		gInts.MaterialSystem = (IMaterialSystem*)MaterialSystemFactory("VMaterialSystem081", NULL);
		gInts.ModelRender    = (IVModelRender*)EngineFactory("VEngineModel016", NULL);
		gInts.Cvar           = (ICvar*)CvarFactory("VEngineCvar004", NULL);
		gInts.EngineTrace    = (IEngineTrace*)EngineFactory("EngineTraceClient003", NULL);
		gInts.RenderView     = (IRenderView*)EngineFactory("VEngineRenderView014", NULL);
		gInts.GameMovement   = (IGameMovement*)(ClientFactory("GameMovement001", NULL));

		g_pConfig->Setup();

		gPlayerVars.hWindow = FindWindowA("Valve001", nullptr);

		//get directx pointer.
		pDevice = **reinterpret_cast<IDirect3DDevice9***>(gSignatures.GetCustomSignature("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

		//get pointer to WndProc
		gPlayerVars.pWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(gPlayerVars.hWindow, GWLP_WNDPROC, LONG_PTR(Hooked_WndProc)));

		gPlayerVars.dwAddFlag               = gSignatures.GetClientSignature("55 8B EC 8B 45 08 09 81");
		DWORD dwUpdatePlayerList            = gSignatures.GetClientSignature("55 8B EC 81 EC ? ? ? ? 53 56 8B D9 E8 ? ? ? ? 8B 8B");
		DWORD dwGetOuterAbsVelocity         = gSignatures.GetClientSignature("55 8B EC 83 EC 34 56 8B F1 8B 0D ? ? ? ? 8B 01");
		DWORD dwMaintainSequenceTransitions = gSignatures.GetClientSignature("55 8B EC 83 EC 0C 56 8B 75 08 57 8B F9 8B CE E8");
		
		DWORD dwEstimateAbsVelocity         = gSignatures.GetClientSignature("55 8B EC 83 EC 0C 56 8B F1 E8 ?? ?? ?? ?? 3B");
		DWORD dwUpdateClientSideAnimations  = gSignatures.GetClientSignature("55 8B EC 83 EC 0C 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 74 10 8B 17 8B CF 8B 92 ? ? ? ? FF D2");
	
		DWORD dwItemPostFrame = gSignatures.GetClientSignature("55 8B EC 83 EC 0C 56 57 8B F9 8B 37");

		//gSignatures.GetClientSignature("56 8B F1 57 8B 3D B0 2C BE ?? 8B 96 BC 01 ?? ?? 83 FA FF 74 1D 8B C2 25 FF 0F ?? ?? C1 E0");
		//gay as fuck relative sig. idk why sig above doesnt work. idk. it leads to the correct place but when i place a trampoline there it just doesnt work.
		DWORD dwTeleportedRel = gSignatures.GetClientSignature("E8 ? ? ? ? 84 C0 75 08 8A 47");
		DWORD dwMask = dwTeleportedRel + 1;
		DWORD dwTeleported = ((*(PDWORD)(dwMask)) + dwMask + 4);
		


		//if called from EnableWorldFog
		gPlayerVars.dwFogReturnAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 01 8B 40 3C FF D0 84 C0 74 16") + 0xD;

		gInts.MoveHelper = **(IMoveHelper***)(gSignatures.GetClientSignature("8B 0D ? ? ? ? 50 8B 45 EC 25") + 0x2);

		//im still grabbing the predictionplayer sig but when you set this in tf2, it doesnt work. havent looked into why but ye.
		gPlayerVars.m_pPredictionPlayer = *reinterpret_cast<CBaseEntity**>(gSignatures.GetClientSignature("89 3D ? ? ? ? F3 0F") + 0x1);
		gPlayerVars.m_pPredictionRandomSeed = *reinterpret_cast<int**>(gSignatures.GetClientSignature("A3 ? ? ? ? 5D C3 55 8B EC 51") + 0x1);

		//grab pointer to demoplayer.
		 DWORD dwDemoPlayerRelative      = gSignatures.GetEngineSignature("8B 0D ? ? ? ? 8B 01 8B 40 18 FF D0 84 C0 75 44");
		 gInts.DemoPlayer                = **(IDemoPlayer***)(dwDemoPlayerRelative + 0x2);
		 gPlayerVars.dwPureReturnAddress = dwDemoPlayerRelative + 0xD;
		
		//grab gamerules
		gInts.GameRules = **(CGameRules***)(gSignatures.GetClientSignature("8B 0D ? ? ? ? 89 45 DC 85") + 0x2);

		//grab clientmode.
		uint32_t* pClientVFTable = *(uint32_t**)gInts.Client;
		gInts.ClientMode = **(ClientModeShared***)(pClientVFTable[10] + 0x5);

		//initialize netvars.
		gNetvars->Initialize();
		
		//initialize cvars.
		gConvars.cl_interp                  = gInts.Cvar->FindVar("cl_interp");
		gConvars.cl_interp_ratio            = gInts.Cvar->FindVar("cl_interp_ratio");
		gConvars.cl_updaterate              = gInts.Cvar->FindVar("cl_updaterate");
		gConvars.cl_cmdrate                 = gInts.Cvar->FindVar("cl_cmdrate");		
		gConvars.sv_minupdaterate           = gInts.Cvar->FindVar("sv_minupdaterate");
		gConvars.sv_maxupdaterate           = gInts.Cvar->FindVar("sv_maxupdaterate");
		gConvars.sv_accelerate              = gInts.Cvar->FindVar("sv_accelerate");
		gConvars.sv_airaccelerate           = gInts.Cvar->FindVar("sv_airaccelerate");
		gConvars.cl_forwardspeed            = gInts.Cvar->FindVar("cl_forwardspeed");
		gConvars.cl_sidespeed               = gInts.Cvar->FindVar("cl_sidespeed");
		gConvars.sv_client_max_interp_ratio = gInts.Cvar->FindVar("sv_client_max_interp_ratio");
		gConvars.sv_client_min_interp_ratio = gInts.Cvar->FindVar("sv_client_min_interp_ratio");
		gConvars.glow_outline_effect_enable = gInts.Cvar->FindVar("glow_outline_effect_enable");

		gConvars.glow_outline_effect_enable->SetValue(1);

		//get targets.
		PVOID pPaintTraverseTarget       = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.Panels, 41));
		PVOID pLevelInitTarget           = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.Client, 5));
		PVOID pFrameStageTarget          = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.Client, 35));
		PVOID pSetViewAnglesTarget       = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.Engine, 20));
		PVOID pOverrideViewTarget        = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.ClientMode, 16));
		PVOID pCreateMoveTarget          = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.ClientMode, 21));
		PVOID pModelRenderTarget         = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.ModelRender, 19));
		PVOID pIsPlayingBackTarget       = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.DemoPlayer, 6));
		PVOID pShouldDrawFogTarget       = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.ClientMode, 15));
		PVOID pRunCommandTarget          = reinterpret_cast<PVOID>(UTIL_GetVirtual(gInts.Prediction, 17));
		PVOID pEndSceneTarget            = reinterpret_cast<PVOID>(UTIL_GetVirtual(pDevice, 42));
		PVOID pGetOuterAbsVelocityTarget = reinterpret_cast<PVOID>(dwGetOuterAbsVelocity);
		PVOID pUpdatePlayerList          = reinterpret_cast<PVOID>(dwUpdatePlayerList);
		PVOID pMaintainSequenceTrans     = reinterpret_cast<PVOID>(dwMaintainSequenceTransitions);
		PVOID pEstimateAbsVelocity       = reinterpret_cast<PVOID>(dwEstimateAbsVelocity);
		PVOID pTeleported                = reinterpret_cast<PVOID>(dwTeleported);
		PVOID pAddFlag                   = reinterpret_cast<PVOID>(gSignatures.GetClientSignature("55 8B EC 8B 45 08 09 81"));
		PVOID pItemPostFrame             = reinterpret_cast<PVOID>(dwItemPostFrame);
		PVOID pUpdateClientSideAnimations = reinterpret_cast<PVOID>(dwUpdateClientSideAnimations);

		

		//PVOID pFogTarget = reinterpret_cast<PVOID>(gSignatures.GetClientSignature("55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53"));
		//PVOID pSkyboxFogTarget = reinterpret_cast<PVOID>(gSignatures.GetClientSignature("55 8B EC 83 EC 10 57 E8"));
		//PVOID pPostProcessTarget = reinterpret_cast<PVOID>(gSignatures.GetClientSignature("55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 33"));
		//PVOID pShouldDrawFogTarget = reinterpret_cast<PVOID>(Get(gInts.ClientMode, 15));
		//PVOID pLevelPostTarget = reinterpret_cast<PVOID>(Get(gInts.Client, 6));
		//PVOID pModelRenderTarget = reinterpret_cast<PVOID>(Get(gInts.ModelRender, 19));

		if (MH_Initialize() != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pPaintTraverseTarget, &Hooked_PaintTraverse, reinterpret_cast<void**>(&oPanel)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pCreateMoveTarget, &Hooked_CreateMove, reinterpret_cast<void**>(&bReturn)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pModelRenderTarget, &Hooked_DrawModelExecute, reinterpret_cast<void**>(&oDrawModel)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pOverrideViewTarget, &Hooked_OverrideView, reinterpret_cast<void**>(&oView)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pFrameStageTarget, &Hooked_FrameStageNotify, reinterpret_cast<void**>(&oFrameStage)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pIsPlayingBackTarget, &Hooked_IsPlayingBack, reinterpret_cast<void**>(&oIsPlayingBack)) != MH_OK)
		{
			return FALSE;
		}

		/*if (MH_CreateHook(pShouldDrawFogTarget, &Hooked_ShouldDrawFog, reinterpret_cast<void**>(&oFog)) != MH_OK)
		{
			return FALSE;
		}*/

		if (MH_CreateHook(pLevelInitTarget, &Hooked_LevelInitPreEntity, reinterpret_cast<void**>(&oLevelInit)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pEndSceneTarget, &Hooked_EndScene, reinterpret_cast<void**>(&oEndScene)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pMaintainSequenceTrans, &Hooked_MaintainSequenceTransitions, nullptr) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pRunCommandTarget, &Hooked_RunCommand, reinterpret_cast<void**>(&oRunCommand)) != MH_OK)
		{
			return FALSE;
		}

		/*if (MH_CreateHook(pEstimateAbsVelocity, &Hooked_EstimateAbsVelocity, reinterpret_cast<void**>(&oEstimateAbsVelocity)) != MH_OK)
		{
			return FALSE;
		}*/

		if (MH_CreateHook(pTeleported, &Hooked_Teleported, reinterpret_cast<void**>(&oTeleported)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pAddFlag, &Hooked_AddFlag, reinterpret_cast<void**>(&oAddFlag)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pItemPostFrame, &Hooked_ItemPostFrame, reinterpret_cast<void**>(&oItemPostFrame)) != MH_OK)
		{
			return FALSE;
		}

		/*if (MH_CreateHook(pUpdateClientSideAnimations, &Hooked_UpdateClientSideAnimations, reinterpret_cast<void**>(&oUpdateClientSideAnimations)) != MH_OK)
		{
			return FALSE;
		}*/

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			return FALSE;
		}

	}

	/*try
	{

	}
	catch (std::exception& e)
	{
		FreeLibraryAndExitThread(static_cast<HMODULE>(hInstance), 0);
	}*/

	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	FreeLibraryAndExitThread(static_cast<HMODULE>(hInstance), 0);
}

DWORD WINAPI UnHook()
{
	//disable glow effect when uninjecting. that way glow doesnt linger. ghetto solution but whatevz.
	gConvars.glow_outline_effect_enable->SetValue(0);

	//restore wndproc
	SetWindowLongPtrA(gPlayerVars.hWindow, GWLP_WNDPROC, LONG_PTR(gPlayerVars.pWndProc));

	MH_Uninitialize();
	MH_DisableHook(MH_ALL_HOOKS);

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hInstance);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (HANDLE hCheatHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)dwMainThread, hInstance, 0, NULL))
			CloseHandle(hCheatHandle);
		break;
	case DLL_PROCESS_DETACH:
		UnHook();
		break;
	}

	return true;
}