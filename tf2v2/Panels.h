#pragma once
#include "SDK.h"

typedef struct CScreenSize_t
{
	int iScreenHeight;
	int iScreenWidth;

} CScreenSize;


void __fastcall Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);

LRESULT STDMETHODCALLTYPE Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
typedef long(__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef HRESULT(STDMETHODCALLTYPE * EndSceneFn)(IDirect3DDevice9*); //a bit ghetto
HRESULT STDMETHODCALLTYPE Hooked_EndScene(IDirect3DDevice9* pDevice);

void Intro();

extern EndSceneFn oEndScene;
extern CScreenSize gScreenSize;