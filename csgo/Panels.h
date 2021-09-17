#pragma once

typedef struct CScreenSize_t
{
	int iScreenHeight;
	int iScreenWidth;

} CScreenSize;
void __fastcall Hooked_DrawModelExecute(PVOID thisptr, PVOID edx, PVOID ctx, PVOID state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
void __fastcall Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
void Intro();

extern CScreenSize gScreenSize;