#include "SDK.h"
#include "Client.h"
#include "Panels.h"

inline UINT Get(PVOID class_, UINT index) 
{
	return (UINT)(*(int**)class_)[index]; 
}

COffsets gOffsets;
CPlayerVariables gVars;
CInterfaces gInts;
CConvars gConvars;

CreateMoveFn bReturn = nullptr;
PanelsFn oPanel = nullptr;
FrameStageFn oFrameStage = nullptr;
OverrideViewFn oView = nullptr;
DrawModelExecuteFn oDrawModel = nullptr;
ShouldDrawFogFn oFog = nullptr;
LevelInitPreEntity oLevelInit = nullptr;
LevelInitPostEntity oLevelPost = nullptr;
IsPlayingBackFn oIsPlayingBack = nullptr;
IsPlayingTimeDemoFn oIsPlayingTimeDemo = nullptr;
ViewmodelFn oViewModel = nullptr;

CreateInterface_t EngineFactory = NULL;
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUIFactory = NULL;
CreateInterface_t VGUI2Factory = NULL;
CreateInterfaceFn MaterialSystemFactory = NULL;
CreateInterface_t CvarFactory = NULL;
CreateInterface_t ServerFactory = NULL;

RecvVarProxyFn fnSequenceProxyFn = NULL;
RecvVarProxyFn oRecvnModelIndex = NULL;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	// Get the knife view model id's
	int iDefaultT = gInts.ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int iDefaultCT = gInts.ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iM9Bayonet = gInts.ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iKarambit = gInts.ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iButterfly = gInts.ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iStilleto = gInts.ModelInfo->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
	int iFlip = gInts.ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
	CBaseEntity* pLocal = GetBaseEntity(me);

	if (gVars.bSkinChanger && pLocal && pLocal->IsAlive())
	{
		if (pData->m_Value.m_Int == iDefaultT || pData->m_Value.m_Int == iDefaultCT)
		{
			pData->m_Value.m_Int = iM9Bayonet;
		}
	}

	oRecvnModelIndex(pData, pStruct, pOut);
}



void Hooked_RecvProxy_Sequence(const CRecvProxyData *pDataConst, void *pStruct, void *pOut)
{
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);
	CDoIReallyFuckingNeedThis* pViewModel = (CDoIReallyFuckingNeedThis*)pStruct;

	if (pViewModel)
	{

		CBaseEntity* pOwner = gInts.EntList->GetClientEntityFromHandle(pViewModel->GetViewModelOwnerHandle());
		//gVars.DEBUG.pOwnerAddress = (DWORD*)pOwner;

		if (pOwner && pOwner->GetIndex() == me)
		{
			//gVars.DEBUG.bPassedSanityChecks = true;

			const DWORD* pModel = gInts.ModelInfo->GetModel(pViewModel->GetModelIndex());
			const char* szModel = gInts.ModelInfo->GetModelName(pModel);

			// Store the current sequence.
			int m_nSequence = pData->m_Value.m_Int;
			gVars.DEBUG.nSequence = m_nSequence;

			if (!strcmp(szModel, "models/weapons/v_knife_stiletto.mdl"))
			{
				gVars.DEBUG.bIsCorrectKnife = true;

				// Fix animations for the Butterfly Knife.
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(12, 13);
					break;
				}
			}

			pData->m_Value.m_Int = m_nSequence;
		}
	}

	//gVars.DEBUG_bPassedSanityChecks = false;
	//gVars.DEBUG.bOriginalWasCalled = false;

	fnSequenceProxyFn(pData, pStruct, pOut);
}

void SequenceHook()
{
	for (ClientClass* pClass = gInts.Client->GetAllClasses(); pClass; pClass = pClass->pNextClass) 
	{
		if (!strcmp(pClass->chName, "CBaseViewModel")) {
			RecvTable* pClassTable = pClass->Table;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;

				// Store the original proxy function.
				fnSequenceProxyFn = static_cast<RecvVarProxyFn>(pProp->m_ProxyFn);

				// Replace the proxy function with our sequence changer.
				pProp->m_ProxyFn = static_cast<RecvVarProxyFn>(Hooked_RecvProxy_Sequence);

				break;
			}

			break;
		}
	}
}


void ViewModelHook()
{
	SequenceHook();

	ClientClass *pClass = gInts.Client->GetAllClasses();
	while (pClass)
	{
		if (!strcmp(pClass->Table->m_pNetTableName, "DT_BaseViewModel"))
		{
			for (int i = 0; i < pClass->Table->m_nProps; i++)
			{
				RecvProp *pProp = &pClass->Table->m_pProps[i];

				if (!strcmp(pProp->m_pVarName, "m_nModelIndex"))
				{
					oRecvnModelIndex = static_cast<RecvVarProxyFn>(pProp->m_ProxyFn);
					pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
				}
			}
		}

		pClass = pClass->pNextClass;
	}

}

DWORD WINAPI dwMainThread(HMODULE hInstance, LPVOID lpArguments )
{

	if (gInts.Client == NULL)
	{
		ClientFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "client.dll" ), "CreateInterface" );
		ServerFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("server.dll"), "CreateInterface");
		EngineFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface");
		VGUIFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vguimatsurface.dll"), "CreateInterface");
		VGUI2Factory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vgui2.dll"), "CreateInterface");
		MaterialSystemFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("MaterialSystem.dll"), "CreateInterface");
		CvarFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vstdlib.dll"), "CreateInterface");
		gInts.Client = ( CHLClient* )ClientFactory( "VClient018", NULL);
		gInts.EntList = ( CEntList* ) ClientFactory( "VClientEntityList003", NULL );
		gInts.Engine = ( EngineClient* ) EngineFactory( "VEngineClient014", NULL );
		gInts.Surface = ( ISurface* ) VGUIFactory( "VGUI_Surface031", NULL );
		gInts.Panels = (IPanel*)VGUI2Factory("VGUI_Panel009", NULL);
		gInts.DebugOverlay = (CDebugOverlay*)(EngineFactory("VDebugOverlay004", NULL));
		gInts.ModelInfo = (IVModelInfo*)EngineFactory("VModelInfoClient004", NULL);
		gInts.MaterialSystem = (IMaterialSystem*)MaterialSystemFactory("VMaterialSystem080", NULL);
		gInts.ModelRender = (IVModelRender*)EngineFactory("VEngineModel016", NULL);
		gInts.Cvar = (ICvar*)CvarFactory("VEngineCvar007", NULL);
		gInts.EngineTrace = (IEngineTrace*)EngineFactory("EngineTraceClient004", NULL);
		gInts.RenderView = (IRenderView*)EngineFactory("VEngineRenderView014", NULL);
		
		//different for csgo.
		gVars.dwFogReturnAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 01 8B 40 44 FF D0 84 C0 74") + 0xD;

		//grab clientmode.
		DWORD dwClientModeAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 01 5D FF 60 30");
		gInts.ClientMode = **(ClientModeShared***)(dwClientModeAddress + 2);

		gInts.Globals = **reinterpret_cast<CGlobals***>((*reinterpret_cast<uintptr_t**>(gInts.Client))[11] + 10);

		//initialize netvars.
		gNetvars = std::make_unique<NetvarTree>();
		NetVarManager->Initialize();
		
		//get targets.
		PVOID pCreateMoveTarget = reinterpret_cast<PVOID>(Get(gInts.ClientMode, 24));
		PVOID pPaintTraverseTarget = reinterpret_cast<PVOID>(Get(gInts.Panels, 41));
	//	PVOID pOverrideViewTarget = reinterpret_cast<PVOID*>(Get(gInts.ClientMode, 18));
	//	PVOID pViewmodelFovTarget = reinterpret_cast<PVOID*>(Get(gInts.ClientMode, 35));
		PVOID pModelRenderTarget = reinterpret_cast<PVOID>(Get(gInts.ModelRender, 21));
	//	PVOID pShouldDrawFogTarget = reinterpret_cast<PVOID>(Get(gInts.ClientMode, 17));
		PVOID pFrameStageTarget = reinterpret_cast<PVOID>(Get(gInts.Client, 37));

		if (MH_Initialize() != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pCreateMoveTarget, &Hooked_CreateMove, reinterpret_cast<void**>(&bReturn)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pPaintTraverseTarget, &Hooked_PaintTraverse, reinterpret_cast<void**>(&oPanel)) != MH_OK)
		{
			return FALSE;
		}

		/*if (MH_CreateHook(pShouldDrawFogTarget, &Hooked_ShouldDrawFog, reinterpret_cast<void**>(&oFog)) != MH_OK)
		{
			return FALSE;
		}*/

		/*if (MH_CreateHook(pOverrideViewTarget, &Hooked_OverrideView, reinterpret_cast<void**>(&oView)) != MH_OK)
		{
			return FALSE;
		}*/

		/*if (MH_CreateHook(pViewmodelFovTarget, &Hooked_GetViewmodelFov, reinterpret_cast<void**>(&oViewModel)) != MH_OK)
		{
			return FALSE;
		}*/

		if (MH_CreateHook(pModelRenderTarget, &Hooked_DrawModelExecute, reinterpret_cast<void**>(&oDrawModel)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_CreateHook(pFrameStageTarget, &Hooked_FrameStageNotify, reinterpret_cast<void**>(&oFrameStage)) != MH_OK)
		{
			return FALSE;
		}

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			return FALSE;
		}

		ViewModelHook();
	}

	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	FreeLibraryAndExitThread(static_cast<HMODULE>(hInstance), 0);
}

DWORD WINAPI UnHook()
{
	MH_Uninitialize();
	MH_DisableHook(MH_ALL_HOOKS);
	NetVarManager->HookProp("DT_BaseViewModel", "m_nModelIndex", oRecvnModelIndex);
	NetVarManager->HookProp("DT_BaseViewModel", "m_nSequence", fnSequenceProxyFn);

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