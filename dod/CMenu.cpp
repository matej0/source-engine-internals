#include "CMenu.h"
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
	//to do: fix hardcoding.
	pCanvas->Canvas(500, 500, 200, 160); //160
	pCanvas->Header("reDodstorm", 500, 495, 200, 14);

	pCanvas->CheckBox("No recoil", 510, 520, 10, 10, &gPlayerVars.bNoRecoil);
	pCanvas->CheckBox("No spread", 600, 520, 10, 10, &gPlayerVars.bNoSpread);

	pCanvas->CheckBox("Name", 510, 540, 10, 10, &gPlayerVars.bName);
	pCanvas->CheckBox("Health", 600, 540, 10, 10, &gPlayerVars.bHealth);

	pCanvas->CheckBox("Box", 510, 560, 10, 10, &gPlayerVars.bBox);
	pCanvas->CheckBox("Teammates", 600, 560, 10, 10, &gPlayerVars.bTeammates);

	pCanvas->CheckBox("Chams", 510, 580, 10, 10, &gPlayerVars.bChams);
	pCanvas->CheckBox("Skeleton", 600, 580, 10, 10, &gPlayerVars.bSkeleton);

	pCanvas->CheckBox("XQZ", 510, 600, 10, 10, &gPlayerVars.bXQZ);

	pCanvas->Slider("FOV", 510, 620, 120, 10, 90, 120, &gPlayerVars.iFov);

	std::vector<std::string> AnalMembers = { "0ff", "Simple", "Specific" };
	pCanvas->ComboBox("Weapon", 510, 640, 120, 15, AnalMembers, &gPlayerVars.iTest);

	//pCanvas->Slider("VFOV", 510, 650, 80, 10, 0, 1000, &gPlayerVars.iViewModelFov);
}