#pragma once
#include "SDK.h"
#include "CDrawManager.h"


/*
CMenu gMenu;
POINT Cursor;
CCanvas* pCanvas;

bool CMenu::MouseInRegion(int x, int y, int x2, int y2)
{
	GetCursorPos(&Cursor);
	return (Cursor.x >= x && Cursor.y >= y && Cursor.x <= x + x2 && Cursor.y <= y + y2);
}

void CMenu::Initialize()
{
	pCanvas = new CCanvas();
}

void CMenu::Draw()
{
	GetCursorPos(&Cursor);

	if (gMenu.MouseInRegion(gMenu.X, gMenu.Y - 15, 222, 15) && GetAsyncKeyState(VK_LBUTTON))
	{
		if (!gMenu.bBeingDragged)
		{
			gMenu.PreviousX = Cursor.x - gMenu.X;
			gMenu.PreviousY = Cursor.y - gMenu.Y;
			gMenu.bBeingDragged = true;
		}

		gMenu.X = Cursor.x;
		gMenu.Y = Cursor.y;

		gMenu.X -= gMenu.PreviousX;
		gMenu.Y -= gMenu.PreviousY;
	}
	else
	{
		gMenu.bBeingDragged = false;
	}


	//to do: fix hardcoding.
	pCanvas->Canvas(gMenu.X, gMenu.Y, 222, 210);
	pCanvas->Header("reDodstorm", gMenu.X, gMenu.Y - 15, 222, 15);

	pCanvas->Tab("AIM", gMenu.X, gMenu.Y, 74, 20, AIM_TAB, &gMenu.TabCurrent);
	pCanvas->Tab("ESP", gMenu.X + 74, gMenu.Y, 74, 20, ESP_TAB, &gMenu.TabCurrent);
	pCanvas->Tab("MISC", gMenu.X + 148, gMenu.Y, 74, 20, MISC_TAB, &gMenu.TabCurrent);

	switch (gMenu.TabCurrent)
	{
	case AIM_TAB:
		pCanvas->CheckBox("Aimbot", gMenu.X + 10, gMenu.Y + 30, 10, 10, &gPlayerVars.bAimbot);
		pCanvas->CheckBox("Autoshoot", gMenu.X + 100, gMenu.Y + 30, 10, 10, &gPlayerVars.bAutoshoot);

		pCanvas->CheckBox("Silent", gMenu.X + 10, gMenu.Y + 50, 10, 10, &gPlayerVars.bSilent);
		pCanvas->CheckBox("Hitscan", gMenu.X + 100, gMenu.Y + 50, 10, 10, &gPlayerVars.bHitscan);
		pCanvas->Slider("Aim FOV", gMenu.X + 10, gMenu.Y + 70, 120, 10, 0, 180, &gPlayerVars.iAimbotFov);

		break;
	case ESP_TAB:
		pCanvas->CheckBox("Name", gMenu.X + 10, gMenu.Y + 30, 10, 10, &gPlayerVars.bName);
		pCanvas->CheckBox("Health", gMenu.X + 100, gMenu.Y + 30, 10, 10, &gPlayerVars.bHealth);

		pCanvas->CheckBox("Box", gMenu.X + 10, gMenu.Y + 50, 10, 10, &gPlayerVars.bBox);
		pCanvas->CheckBox("Weapon", gMenu.X + 100, gMenu.Y + 50, 10, 10, &gPlayerVars.bWeapon);

		pCanvas->CheckBox("Skeleton", gMenu.X + 10, gMenu.Y + 70, 10, 10, &gPlayerVars.bSkeleton);
		pCanvas->CheckBox("Buildings", gMenu.X + 100, gMenu.Y + 70, 10, 10, &gPlayerVars.bBuildings);

		pCanvas->CheckBox("Other", gMenu.X + 10, gMenu.Y + 90, 10, 10, &gPlayerVars.bOther);
		pCanvas->CheckBox("Teammates", gMenu.X + 100, gMenu.Y + 90, 10, 10, &gPlayerVars.bTeammates);

		pCanvas->CheckBox("Conds", gMenu.X + 10, gMenu.Y + 110, 10, 10, &gPlayerVars.bConditions);
		pCanvas->CheckBox("BT Chams", gMenu.X + 100, gMenu.Y + 110, 10, 10, &gPlayerVars.bBacktrackChams);

		pCanvas->CheckBox("Chams", gMenu.X + 10, gMenu.Y + 130, 10, 10, &gPlayerVars.bChams);
		pCanvas->CheckBox("XQZ", gMenu.X + 100, gMenu.Y + 130, 10, 10, &gPlayerVars.bXQZ);

		pCanvas->CheckBox("Radar", gMenu.X + 10, gMenu.Y + 150, 10, 10, &gPlayerVars.bRadar);
		pCanvas->CheckBox("Team color", gMenu.X + 100, gMenu.Y + 150, 10, 10, &gPlayerVars.bTeamColored);
		pCanvas->Slider("Size", gMenu.X + 10, gMenu.Y + 170, 120, 10, 20, 200, &gPlayerVars.iRadarSize);

		break;
	case MISC_TAB:
		pCanvas->CheckBox("Backtrack", gMenu.X + 10, gMenu.Y + 30, 10, 10, &gPlayerVars.bBacktracking);
		pCanvas->CheckBox("Bunny-hop", gMenu.X + 100, gMenu.Y + 30, 10, 10, &gPlayerVars.bBhop);
		pCanvas->CheckBox("Auto-stab", gMenu.X + 10, gMenu.Y + 50, 10, 10, &gPlayerVars.bAutoStab);
		pCanvas->CheckBox("Spectators", gMenu.X + 100, gMenu.Y + 50, 10, 10, &gPlayerVars.bSpecList);
		pCanvas->Slider("FOV", gMenu.X + 10, gMenu.Y + 70, 120, 10, 90, 120, &gPlayerVars.iFov);

		break;
	}
}

*/
enum Tabs
{
	AIM_TAB,
	ESP_TAB,
	MISC_TAB
};

class CMenu
{
public:
	bool MouseInRegion(int x, int y, int x2, int y2);
	//void Initialize();
	void Draw();
	int TabCurrent = 0;
	int X = 500;
	int Y = 500;
	int PreviousX = 0;
	int PreviousY = 0;
	bool bBeingDragged = true;
	bool m_bIsOpen;
	ImFont* m_pIconsFont;
	ImFont* m_pRuda;
};

extern CMenu gMenu;

/*class CCanvas
{
public:
	void Header(std::string szTitle, int x, int y, int w, int h)
	{
		gDrawManager.DrawRect(x, y, w, h, COLORCODE(255, 120, 25, 250));
		gDrawManager.DrawString(gDrawManager.GetFont(), x + 2, y, COLORCODE(255, 255, 255, 255), szTitle.c_str()); //Draw the string AFTER the rectangle.
	}

	void Canvas(int x, int y, int w, int h)
	{
		gDrawManager.DrawRect(x, y, w, h, COLORCODE(45, 45, 45, 240));
	}

	void Tab(std::string szTitle, int x, int y, int w, int h, int num, int* current)
	{
		static DWORD dwColor;

		static bool bPressed = false;
		static bool bHovering = false;

		if (!GetAsyncKeyState(VK_LBUTTON) && gMenu.MouseInRegion(x, y, w, h))
		{
			if (bPressed)
				*current = num;

			bPressed = false;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && gMenu.MouseInRegion(x, y, w, h) && !bPressed)
			bPressed = true;

		//if (gMenu.MouseInRegion(x, y, w, h))
			//dwColor = COLORCODE(255, 120, 25, 200);

		if (*current == num)
			dwColor = COLORCODE(255, 120, 25, 255);
		else
			dwColor = COLORCODE(255, 255, 255, 255);

		//gDrawManager.DrawRect(x, y, w, h, dwColor);
		gDrawManager.DrawStringA(gDrawManager.GetFont(), true, x + w / 2, y + 2, dwColor, szTitle.c_str());

	}

	void CheckBox(std::string szTitle, int x, int y, int w, int h, bool* bSwitch)
	{
		static bool bPressed = false;

		static DWORD dwColor;

		if (!GetAsyncKeyState(VK_LBUTTON) && gMenu.MouseInRegion(x, y, w, h))
		{
			if (bPressed)
				*bSwitch = !*bSwitch;

			bPressed = false;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && gMenu.MouseInRegion(x, y, w, h) && !bPressed)
			bPressed = true;

		*bSwitch ? dwColor = COLORCODE(255, 120, 25, 250) : dwColor = COLORCODE(10, 10, 10, 250);

		gDrawManager.DrawRect(x, y, w, h, dwColor);
		gDrawManager.DrawString(gDrawManager.GetFont(), x + w + 10, y - 3, COLORCODE(255, 255, 255, 255), szTitle.c_str());
	}

	void Slider(std::string szTitle, int x, int y, int w, int h, int min, int max, int* iControl)
	{
		POINT C;
		GetCursorPos(&C);

		//clamping
		if (*iControl >= max)
			*iControl = max;

		if (*iControl <= min)
			*iControl = min;

		LONG iDelta = C.x - x;
		float flMultiplier = iDelta / static_cast<float>(w);

		if (GetAsyncKeyState(VK_LBUTTON) && gMenu.MouseInRegion(x, y, w, h))
		{
			*iControl = flMultiplier * (max - min) + min;
		}

		gDrawManager.DrawRect(x, y, w, h, COLORCODE(10, 10, 10, 250));
		gDrawManager.DrawRect(x, y, (*iControl - min) * w / (max - min), h, COLORCODE(255, 120, 25, 250));
		gDrawManager.DrawString(gDrawManager.GetFont(), x + w + 5, y - 3, COLORCODE(255, 255, 255, 255), (szTitle + ": " + std::to_string(*iControl)).c_str());
	}

	void GroupBox(std::string szTitle, std::vector<std::string> members, int* member)
	{
		//to do.
	}
};*/

