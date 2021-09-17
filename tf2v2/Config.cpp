#include "Config.h"

void CConfig::Setup()
{
	SetupValue(gPlayerVars.bAimbot, false, "Aimbot", "Enabled");
	SetupValue(gPlayerVars.bSilent, false, "Aimbot", "Silent");
	SetupValue(gPlayerVars.bAutoshoot, false, "Aimbot", "Autoshoot");
	SetupValue(gPlayerVars.bHitscan, false, "Aimbot", "Hitscan");
	SetupValue(gPlayerVars.iAimbotFov, 0, "Aimbot", "FOV");

	SetupValue(gPlayerVars.bEspPlayers, false, "ESP", "PlayersEnabled");
	SetupValue(gPlayerVars.bName, false, "ESP", "Name");
	SetupValue(gPlayerVars.bBox, false, "ESP", "Box");
	SetupValue(gPlayerVars.bHealth, false, "ESP", "Health");
	SetupValue(gPlayerVars.bWeapon, false, "ESP", "Weapon");
	SetupValue(gPlayerVars.bChams, false, "ESP", "Chams");
	SetupValue(gPlayerVars.bXQZ, false, "ESP", "XQZ");
	SetupValue(gPlayerVars.bBacktrackChams, false, "ESP", "BTChams");
	SetupValue(gPlayerVars.bSkeleton, false, "ESP", "Skeleton");

	SetupValue(gPlayerVars.bEspBuildings, false, "ESP", "BuildingsEnabled");
	SetupValue(gPlayerVars.bNameBuildings, false, "ESP", "BuildingsName");
	SetupValue(gPlayerVars.bBoxBuildings, false, "ESP", "BuildingsBox");
	SetupValue(gPlayerVars.bHealthBuildings, false, "ESP", "BuildingsHealth");
	SetupValue(gPlayerVars.bBuildingLevel, false, "ESP", "BuildingsLevel");

	SetupValue(gPlayerVars.bTeammates, false, "ESP", "TeamCheck");

	SetupValue(gPlayerVars.bBacktracking, false, "MISC", "Backtracking");
	SetupValue(gPlayerVars.bBhop, false, "MISC", "Bhop");
	SetupValue(gPlayerVars.bAutoStab, false, "MISC", "Autostab");
	SetupValue(gPlayerVars.bSpecList, false, "MISC", "SpecList");
	SetupValue(gPlayerVars.iFov, 90, "MISC", "FOV");
}

void CConfig::SetupValue(int& value, int def, std::string category, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue< int >(category, name, &value));
}

void CConfig::SetupValue(float& value, float def, std::string category, std::string name)
{
	value = def;
	floats.push_back(new ConfigValue< float >(category, name, &value));
}

void CConfig::SetupValue(bool& value, bool def, std::string category, std::string name)
{
	value = def;
	bools.push_back(new ConfigValue< bool >(category, name, &value));
}

void CConfig::Save()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;
		
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + "\\reDodstorm\\";
	    file = std::string(path) +  "\\reDodstorm\\cfg.ini";
	}

	CreateDirectory(folder.c_str(), NULL);

	for (auto value : ints)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : floats)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str());
}

void CConfig::Load()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + "\\reDodstorm\\";
		file = std::string(path) + "\\reDodstorm\\cfg.ini";
	}

	CreateDirectory(folder.c_str(), NULL);
	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atoi(value_l);
	}

	for (auto value : floats)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atof(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = !strcmp(value_l, "true");
	}
}

CConfig* g_pConfig = new CConfig();
