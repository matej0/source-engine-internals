#pragma once
#include "SDK.h"
#include "CDrawManager.h"

class CMenu
{
public:
	bool MouseInRegion(int x, int y, int x2, int y2);
	void Initialize();
	void Draw();
	bool IsMenuOpen;
};
extern CMenu gMenu;

//my first attempt at a menu. please dont bully.

class CCanvas
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

	void CheckBox(std::string szTitle, int x, int y, int w, int h, bool* bSwitch)
	{
		//credits some stackoverflow thread.
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

	void ComboBox(std::string szTitle, int x, int y, int w, int h, std::vector<std::string> arrMembers, int* iControl)
	{
		static bool bPressed = false;
		static bool bSwitch;
		DWORD dwControlColor;

		if (!GetAsyncKeyState(VK_LBUTTON) && gMenu.MouseInRegion(x, y, w, h))
		{
			if (bPressed)
				bSwitch = !bSwitch;

			bPressed = false;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && gMenu.MouseInRegion(x, y, w, h) && !bPressed)
			bPressed = true;

		gDrawManager.DrawRect(x, y, w, h, COLORCODE(10, 10, 10, 250));
		gDrawManager.DrawString(gDrawManager.GetFont(), x + 2, y, COLOR_WHITE, arrMembers.at(gPlayerVars.iTest).c_str());
		gDrawManager.DrawString(gDrawManager.GetFont(), x + w + 5, y, COLORCODE(255, 255, 255, 255), szTitle.c_str());

		if (bSwitch)
		{
			int iOffset = h;

			for (int i = 0; i < arrMembers.size(); i++)
			{
				if (gMenu.MouseInRegion(x, y + h * (i + 1), w, h))
				{
					dwControlColor = COLORCODE(255, 120, 25, 250);

					if (GetAsyncKeyState(VK_LBUTTON))
					{
						*iControl = i;
						bSwitch = false;
					}
						
				}
				else
				{
					dwControlColor = COLOR_WHITE;
				}

				gDrawManager.DrawRect(x, y + iOffset, w, h, COLORCODE(10, 10, 10, 250));
				gDrawManager.DrawString(gDrawManager.GetFont(), x + 2, y + iOffset, dwControlColor, arrMembers.at(i).c_str());
				iOffset += h;
			}
		}
	}

	void ColorPicker()
	{
		POINT C;
		GetCursorPos(&C);

		HDC hScreenDC = GetDC(NULL);
		COLORREF Penis = GetPixel(hScreenDC, C.x, C.y);
		ReleaseDC(NULL, hScreenDC);
		int R = GetRValue(Penis);
		int G = GetGValue(Penis);
		int B = GetBValue(Penis);

		gDrawManager.DrawRect(500, 100, 50, 50, COLORCODE(R, G, B, 255));
	}
};

