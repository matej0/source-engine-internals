#include "CMenu.h"

CCheatMenu gCheatMenu;

int ScreenH;
int ScreenW;

char* szKeyNames[] = {
	"None", "Mouse 1", "Mouse 2", "Mouse 3", "Mouse 4", "Mouse 5", "Shift", "Alt", "F",
};

char* szHitboxes[] = {
	"Head", "Body",
};

char* szRageHitboxes[] = {
	"Min", "Mid", "Max",
};

char* szSmoothLevels[] = 
{
	"OFF", "Low", "High"
};

void CCheatMenu::HandleMenuInput()
{
	if (GetAsyncKeyState(VK_INSERT) & 1) //insert
	{
		gCheatMenu.bMenuActive = !gCheatMenu.bMenuActive;
	}

	if (gCheatMenu.bMenuActive)
	{
		if (GetAsyncKeyState(VK_UP) & 1) // Up
		{
			if (gCheatMenu.iMenuIndex > 0)
				gCheatMenu.iMenuIndex--;
			else
				gCheatMenu.iMenuIndex = gCheatMenu.iMenuItems - 1;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 1) // Down
		{
			if (gCheatMenu.iMenuIndex < gCheatMenu.iMenuItems - 1)
				gCheatMenu.iMenuIndex++;
			else
				gCheatMenu.iMenuIndex = 0;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 1) // Left
		{
			if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value)
			{
				gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] -= gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flStep;
				if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] < gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMin)
					gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] = gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMax;
			}
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 1) // Right
		{
			if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value)
			{
				gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] += gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flStep;
				if (gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] > gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMax)
					gCheatMenu.pMenu[gCheatMenu.iMenuIndex].value[0] = gCheatMenu.pMenu[gCheatMenu.iMenuIndex].flMin;
			}
		}

	}
}

int CCheatMenu::AddItem(int nIndex, char szTitle[30], float* value, float flMin, float flMax, float flStep, bool isClassSwitch)
{
	strcpy(pMenu[nIndex].szTitle, szTitle);
	pMenu[nIndex].value = value;
	pMenu[nIndex].flMin = flMin;
	pMenu[nIndex].flMax = flMax;
	pMenu[nIndex].flStep = flStep;
	pMenu[nIndex].isClassSwitch = isClassSwitch;
	return (nIndex + 1);
}

void CCheatMenu::Render(void)
{
	gInts.Engine->GetScreenSize(ScreenW, ScreenH);

	int i = 0;

	i = AddItem(i, " esp", &gVars.bESP, 0, 1, 1, false);
	i = AddItem(i, "   show info", &gVars.bShowInfo, 0, 1, 1, false);
	i = AddItem(i, "   healthbar", &gVars.bHealthBar, 0, 1, 1, false);
	i = AddItem(i, "   box", &gVars.bBox, 0, 1, 1, false);
	i = AddItem(i, "   chams", &gVars.bChams, 0, 1, 1, false);
	i = AddItem(i, "   xqz", &gVars.bXQZChams, 0, 1, 1, false);
	//i = AddItem(i, "   bt chams", &gVars.bLagChams, 0, 1, 1, false);
	i = AddItem(i, "   skeleton", &gVars.bSkeleton, 0, 1, 1, false);

	i = AddItem(i, " aimbot", &gVars.bAimbot, 0, 1, 1, false);
	i = AddItem(i, "   smooth", &gVars.bSmooth, 0, 2, 1, false);
	i = AddItem(i, "   fov", &gVars.bFOV, 0, 30, 1, false);
	i = AddItem(i, "   rcs", &gVars.bRCS, 0, 1, 1, false);
	i = AddItem(i, "   bone", &gVars.iBone, 0, 1, 1, false);
	i = AddItem(i, "   rage", &gVars.bRageMode, 0, 1, 1, false);
	i = AddItem(i, "     silent", &gVars.bSilent, 0, 1, 1, false);
	i = AddItem(i, "     hitboxes", &gVars.iHitboxes, 0, 2, 1, false);
	i = AddItem(i, "     multipoint", &gVars.bMultiPoint, 0, 1, 1, false);
	i = AddItem(i, "     scale", &gVars.nMultiPointScale, 10, 100, 10, false);
	i = AddItem(i, "     autoshoot", &gVars.bAutoShoot, 0, 1, 1, false);
	i = AddItem(i, "     autowall", &gVars.bAutowall, 0, 1, 1, false);
	i = AddItem(i, "     dmg", &gVars.nAutoWallDmg, 0, 1, 1, false);
	i = AddItem(i, " triggerbot", &gVars.bTriggerbot, 0, 1, 1, false);
	i = AddItem(i, " bunnyhop", &gVars.bBunnyhop, 0, 1, 1, false);
	i = AddItem(i, " skinchanger", &gVars.bSkinChanger, 0, 1, 1, false);
	//i = AddItem(i, " fov", &gVars.iFov, 90, 120, 1, false);
	//i = AddItem(i, " vmfov", &gVars.iVMFov, 54, 120, 1, false);
	i = AddItem(i, " backtrack", &gVars.bBacktracking, 0, 1, 1, false);
	i = AddItem(i, "   ticks", &gVars.nBacktrackTicks, 1, 12, 1, false);

	iMenuItems = i;
}

void CCheatMenu::DrawMenu(void)
{
	int x = gVars.iMenu_Pos_X,
		xx = x + 115,
		y = gVars.iMenu_Pos_Y,
		w = 150,
		h = 20;

	static DWORD clr = COLORCODE(255, 20, 147, 255);
	static DWORD off = COLORCODE(255, 0, 0, 255);
	static DWORD on = COLORCODE(0, 255, 0, 255);

	gDrawManager.DrawRect(x, y - (h + 4), w, iMenuItems * h + 21, COLORCODE(30, 30, 30, 200));
	gDrawManager.OutlineRect(x, y - (h + 4), w, iMenuItems * h + 21, COLORCODE(0, 0, 0, 255));

	gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, x + 10, y - 22, COLOR_WHITE, "skeethook");

	for (int i = 0; i < iMenuItems; i++)
	{
		if (i != iMenuIndex)
		{
			if (pMenu[i].isClassSwitch)
			{
				if (!(pMenu[i].value[0]))
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, x + 4, y + (h * i), on, "[+] %s", pMenu[i].szTitle);
				}
				else if (pMenu[i].value[0])
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, x + 4, y + (h * i), off, "[-] %s", pMenu[i].szTitle);
				}
			}
			else
			{
				gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, x + 4, y + (h * i), COLOR_WHITE, pMenu[i].szTitle);

				if (!strcmp(pMenu[i].szTitle, "   bone"))
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), on, "%s", pMenu[i].value[0] ? "head" : "body");
				}
				else if (!strcmp(pMenu[i].szTitle, "   smooth"))
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), pMenu[i].value[0] ? on : off, "%s", szSmoothLevels[int(pMenu[i].value[0])]);
				}
				else if (!strcmp(pMenu[i].szTitle, "     hitboxes"))
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), pMenu[i].value[0] ? on : off, "%s", szRageHitboxes[int(pMenu[i].value[0])]);
				}
				else if (pMenu[i].flMax == 1)
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), pMenu[i].value[0] ? on : off, pMenu[i].value[0] ? "ON" : "OFF");
				}
				else
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), pMenu[i].value[0] >= 0 ? on : off, "%0.0f", pMenu[i].value[0]);
				}
			}
		}
		else
		{
			//gDrawManager.DrawRect(x + 1, y + (h * i), w - 2, h, COLORCODE(255, 255, 255, 80));

			if (pMenu[i].isClassSwitch)
			{
				if (!(pMenu[i].value[0]))
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, x + 4, y + (h * i), clr, "[+] %s", pMenu[i].szTitle);
				}
				else if (pMenu[i].value[0])
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, x + 4, y + (h * i), clr, "[-] %s", pMenu[i].szTitle);
				}
			}
			else
			{
				gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, x + 4, y + (h * i), clr, pMenu[i].szTitle);

				if (!strcmp(pMenu[i].szTitle, "   bone"))
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), on, "%s", pMenu[i].value[0] ? "head" : "body");
				}
				else if (!strcmp(pMenu[i].szTitle, "   smooth"))
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), on, "%s", szSmoothLevels[int(pMenu[i].value[0])]);
				}
				else if (!strcmp(pMenu[i].szTitle, "     hitboxes"))
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), on, "%s", szRageHitboxes[int(pMenu[i].value[0])]);
				}
				else if (pMenu[i].flMax == 1)
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), pMenu[i].value[0] ? on : off, pMenu[i].value[0] ? "ON" : "OFF");
				}
				else
				{
					gDrawManager.DrawStringA(gDrawManager.GetAltFont(), false, xx, y + (h * i), pMenu[i].value[0] >= 0 ? on : off, "%0.0f", pMenu[i].value[0]);
				}
			}
		}
	}
}