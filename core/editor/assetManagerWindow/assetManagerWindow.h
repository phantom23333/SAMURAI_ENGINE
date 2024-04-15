#pragma once

#include "Config.h"

#include "settings/log.h"
#include <containerManager/containerManager.h>
#include <filesystem>
#include <IconsForkAwesome.h>
#include <imgui.h>
#include <string>
#include <windowGraphic/gui.h>



namespace samurai
{


	struct AssetManagerWindow
	{

		void init(samurai::samuraiImgui::ImGuiIdsManager &idManager);

		void update(bool& open, ContainerManager& containerManager, LoadedDll& currentDll,
		            samurai::LogManager& logManager, samurai::samuraiImgui::ImGuiIdsManager& imguiIDsManager);

		static constexpr char *ICON = ICON_FK_FILES_O;
		static constexpr char *NAME = "Asset manager";
		static constexpr char *ICON_NAME = ICON_FK_FILES_O " Asset manager";

		int imguiId = 0;
		char searchText[100] = {};

		std::filesystem::path currentPath = SAMURAI_RESOURCES_PATH;
	};

}
