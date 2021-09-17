#include "CMenu.h"
#include "Config.h"
CMenu gMenu;

POINT Cursor;

bool CMenu::MouseInRegion(int x, int y, int x2, int y2)
{
	GetCursorPos(&Cursor);
	return (Cursor.x >= x && Cursor.y >= y && Cursor.x <= x + x2 && Cursor.y <= y + y2);
}

void CMenu::Draw()
{	
	ImGui::SetNextWindowSize(ImVec2{ 300, 220 }, ImGuiCond_Once);

	ImGui::Begin("reDodstorm", &gMenu.m_bIsOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

	if (ImGui::BeginTabBar("##TABS", ImGuiTabBarFlags_NoTooltip))
	{

		//didnt know these tabs even existed
		if (ImGui::BeginTabItem("AIM"))
		{
		
			ImGui::Checkbox("Enable", &gPlayerVars.bAimbot);
			ImGui::SameLine(150.f);
			ImGui::Checkbox("Silent", &gPlayerVars.bSilent);

			ImGui::Checkbox("Hitscan", &gPlayerVars.bHitscan);
			ImGui::SameLine(150.f);
			ImGui::Checkbox("Autoshoot", &gPlayerVars.bAutoshoot);

			ImGui::SliderInt("FOV", &gPlayerVars.iAimbotFov, 0, 180);

			static const char* szIgnores[] = { "Cloaked", "Ubered", "Disguised", "Taunting", "Bonked", };
			static std::string strPreview = "";

			if (ImGui::BeginCombo("Ignore", strPreview.c_str()))
			{
				strPreview = "";
				std::vector<std::string> rgItems;
				for (size_t i = 0; i < IM_ARRAYSIZE(szIgnores); i++)
				{
					ImGui::Selectable(szIgnores[i], &gPlayerVars.bAimIgnores[i], ImGuiSelectableFlags_DontClosePopups);

					if (gPlayerVars.bAimIgnores[i])
						rgItems.push_back(szIgnores[i]);
				}

				for (size_t i = 0; i < rgItems.size(); i++)
				{
					if (rgItems.size() == 1)
						strPreview += rgItems[i];
					else if (!(i == rgItems.size() - 1))
						strPreview += rgItems[i] + ",";
					else
						strPreview += rgItems[i];
				}
				
				ImGui::EndCombo();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("ESP"))
		{
			static int iFilter = 0;
			static const char* szFilter[] = { "Players", "Buildings"};

			ImGui::PushItemWidth(100.f);
			ImGui::Combo("##FILTER", &iFilter, szFilter, IM_ARRAYSIZE(szFilter));
			ImGui::SameLine();


			switch (iFilter)
			{
			case 0:
			{
				ImGui::Checkbox("Enable", &gPlayerVars.bEspPlayers);
				ImGui::Separator();
				ImGui::Checkbox("Name", &gPlayerVars.bName);
				ImGui::SameLine(150.f);
				ImGui::Checkbox("Box", &gPlayerVars.bBox);

				ImGui::Checkbox("Health", &gPlayerVars.bHealth);
				ImGui::SameLine(150.f);
				ImGui::Checkbox("Weapon", &gPlayerVars.bWeapon);

				ImGui::Checkbox("Skeleton", &gPlayerVars.bSkeleton);
				ImGui::SameLine(150.f);
				ImGui::Checkbox("Conds", &gPlayerVars.bConditions);

				ImGui::Checkbox("Chams", &gPlayerVars.bChams);
				ImGui::SameLine();
				ImGui::ColorEdit3("##CHAMCLR", gPlayerVars.clrVisibleChams, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine(150.f);
				ImGui::Checkbox("XQZ", &gPlayerVars.bXQZ);
				ImGui::SameLine();
				ImGui::ColorEdit3("##CHAMCLRXQZ", gPlayerVars.clrXQZChams, ImGuiColorEditFlags_NoInputs);

				ImGui::Checkbox("BT Chams", &gPlayerVars.bBacktrackChams);
			}
			break;
			case 1:
			{
				ImGui::Checkbox("Enable", &gPlayerVars.bEspBuildings);
				ImGui::Separator();
				ImGui::Checkbox("Name", &gPlayerVars.bNameBuildings);
				ImGui::SameLine(150.f);
				ImGui::Checkbox("Box", &gPlayerVars.bBoxBuildings);

				ImGui::Checkbox("Health", &gPlayerVars.bHealthBuildings);
				ImGui::SameLine(150.f);
				ImGui::Checkbox("Level", &gPlayerVars.bBuildingLevel);
			}
			break;
			}
			
			ImGui::Checkbox("Teammates", &gPlayerVars.bTeammates);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("MISC"))
		{
			ImGui::Checkbox("Backtrack", &gPlayerVars.bBacktracking);
			ImGui::SameLine(150.f);
			ImGui::Checkbox("Bunny-hop", &gPlayerVars.bBhop);

			ImGui::Checkbox("Auto-stab", &gPlayerVars.bAutoStab);
			ImGui::SameLine(150.f);
			ImGui::Checkbox("Spectators", &gPlayerVars.bSpecList);

			ImGui::SliderInt("FOV", &gPlayerVars.iFov, 70, 120);

			if (ImGui::Button("Save"))
				g_pConfig->Save();

			ImGui::SameLine();

			if (ImGui::Button("Load"))
				g_pConfig->Load();


			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		//ImGui::Button(ICON_FA_COG);
	}

	ImGui::End();
}