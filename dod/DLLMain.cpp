#include "SDK.h"
#include "Client.h"
#include "Panels.h"
//doubt this will ever see the light of day ( unknowncheats ).
unsigned int Get(void* class_, unsigned int index) { return (unsigned int)(*(int**)class_)[index]; }



COffsets gOffsets;
CPlayerVariables gPlayerVars;
CInterfaces gInts;
CFriends gFriends;
CConvars gConvars;

CreateMoveFn bReturn = nullptr;
PanelsFn oPanel = nullptr;
FrameStageFn oFrameStage = nullptr;
OverrideViewFn oView = nullptr;
RunCommandFn oRunCommand = nullptr;
DrawModelExecuteFn oDrawModel = nullptr;
EnableWorldFogFn oWorldFog = nullptr;
ShouldDrawFogFn oFog = nullptr;
LevelInitPreEntity oLevelInit = nullptr;
LevelInitPostEntity oLevelPost = nullptr;

CreateInterface_t EngineFactory = NULL;
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUIFactory = NULL;
CreateInterface_t VGUI2Factory = NULL;
CreateInterfaceFn MaterialSystemFactory = NULL;
CreateInterface_t CvarFactory = NULL;

//nanohack O.o
DECLVHOOK(void, SetViewAngles, (Vector &a))
{
	if (gPlayerVars.bNoRecoil)
	{
		if (get_SI<CBaseEntity*>() == GetBaseEntity(me))
			return;
	}

	oSetViewAngles(a);
}



DWORD WINAPI dwMainThread(HMODULE hInstance, LPVOID lpArguments )
{

	if (gInts.Client == NULL) //Prevent repeat callings.
	{
		ClientFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "client.dll" ), "CreateInterface" );
		gInts.Client = ( CHLClient* )ClientFactory( "VClient017", NULL);
		gInts.EntList = ( CEntList* ) ClientFactory( "VClientEntityList003", NULL );
		EngineFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "engine.dll" ), "CreateInterface" );
		gInts.Engine = ( EngineClient* ) EngineFactory( "VEngineClient013", NULL );
		VGUIFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "vguimatsurface.dll" ), "CreateInterface" );
		gInts.Surface = ( ISurface* ) VGUIFactory( "VGUI_Surface030", NULL );
		VGUI2Factory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vgui2.dll"), "CreateInterface");
		gInts.Prediction = (CPrediction *)(ClientFactory("VClientPrediction001", NULL));
		gInts.Panels = (IPanel*)VGUI2Factory("VGUI_Panel009", NULL);
		gInts.DebugOverlay = (CDebugOverlay*)(EngineFactory("VDebugOverlay003", NULL));
		gInts.Globals = *reinterpret_cast<CGlobals**>(gSignatures.GetEngineSignature("A1 ? ? ? ? 8B 11 68") + 8);
		gInts.ModelInfo = (IVModelInfo*)EngineFactory("VModelInfoClient006", NULL);
		MaterialSystemFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("MaterialSystem.dll"), "CreateInterface");
		gInts.MaterialSystem = (IMaterialSystem*)MaterialSystemFactory("VMaterialSystem080", NULL);
		gInts.ModelRender = (IVModelRender*)EngineFactory("VEngineModel016", NULL);
		CvarFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vstdlib.dll"), "CreateInterface");
		gInts.Cvar = (ICvar*)CvarFactory("VEngineCvar004", NULL);
		gInts.EngineTrace = (IEngineTrace*)EngineFactory("EngineTraceClient003", NULL);
		gInts.RenderView = (IRenderView*)EngineFactory("VEngineRenderView014", NULL);

		gPlayerVars.dwFogReturnAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 01 8B 40 3C FF D0 84 C0 74 16") + 0xD;

		//grab clientmode.
		uint32_t* pClientVFTable = *(uint32_t**)gInts.Client;
		gInts.ClientMode = **(ClientModeShared***)(pClientVFTable[10] + 0x5);

		//initialize netvars.
		gNetvars = std::make_unique<NetvarTree>();

		//initialize cvars.
		gConvars.cl_interp = gInts.Cvar->FindVar("cl_interp");
		gConvars.cl_interp_ratio = gInts.Cvar->FindVar("cl_interp_ratio");
		gConvars.cl_updaterate = gInts.Cvar->FindVar("cl_updaterate");
		gConvars.cl_cmdrate = gInts.Cvar->FindVar("cl_cmdrate");	
		gConvars.sv_showimpacts = gInts.Cvar->FindVar("sv_showimpacts");
		gConvars.r_viewmodelfov = gInts.Cvar->FindVar("r_viewmodelfov");
	

		//RemoveCommandLimitations();

		//get targets.
		PVOID pPaintTraverseTarget = reinterpret_cast<PVOID>(Get(gInts.Panels, 41));
		PVOID pLevelInitTarget = reinterpret_cast<PVOID>(Get(gInts.Client, 5));
		PVOID pLevelPostTarget = reinterpret_cast<PVOID>(Get(gInts.Client, 6));
		PVOID pFrameStageTarget = reinterpret_cast<PVOID>(Get(gInts.Client, 35));
		PVOID pSetViewAnglesTarget = reinterpret_cast<PVOID>(Get(gInts.Engine, 20));
		PVOID pModelRenderTarget = reinterpret_cast<PVOID>(Get(gInts.ModelRender, 19));
		PVOID pShouldDrawFogTarget = reinterpret_cast<PVOID>(Get(gInts.ClientMode, 15));
		PVOID pOverrideViewTarget = reinterpret_cast<PVOID*>(Get(gInts.ClientMode, 16));
		PVOID pCreateMoveTarget = reinterpret_cast<PVOID>(Get(gInts.ClientMode, 21));
		PVOID pPostProcessTarget = reinterpret_cast<PVOID>(gSignatures.GetClientSignature("55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 33"));
		PVOID pFogTarget = reinterpret_cast<PVOID>(gSignatures.GetClientSignature("55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53"));

		//causes retardation on some maps. IDK why.
		//PVOID pFogTarget = reinterpret_cast<PVOID>(gSignatures.GetClientSignature("55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53"));
		//PVOID pSkyboxFogTarget = reinterpret_cast<PVOID>(gSignatures.GetClientSignature("55 8B EC 83 EC 10 57 E8"));
	    //PVOID pViewmodelFOVTarget = reinterpret_cast<PVOID>(Get(gInts.ClientMode, 32));

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

		if (MH_CreateHook(pSetViewAnglesTarget, &Hooked_SetViewAngles, reinterpret_cast<void**>(&oSetViewAngles)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pFrameStageTarget, &Hooked_FrameStageNotify, reinterpret_cast<void**>(&oFrameStage)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pLevelInitTarget, &Hooked_LevelInitPreEntity, reinterpret_cast<void**>(&oLevelInit)) != MH_OK)
		{
			return FALSE;
		}

		/*if (MH_CreateHook(pShouldDrawFogTarget, &Hooked_ShouldDrawFog, reinterpret_cast<void**>(&oFog)) != MH_OK)
		{
			return FALSE;
		}*/

		if (MH_CreateHook(pFogTarget, &Hooked_EnableWorldFog, reinterpret_cast<void**>(&oWorldFog)) != MH_OK)
		{
			return FALSE;
		}

		/*if (MH_CreateHook(pPostProcessTarget, &Hooked_DoEnginePostProcessing, nullptr) != MH_OK)
		{
			return FALSE;
		}*/

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			return FALSE;
		}
	}

	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	FreeLibraryAndExitThread(static_cast<HMODULE>(hInstance), 0);
}

DWORD WINAPI UnHook()
{
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