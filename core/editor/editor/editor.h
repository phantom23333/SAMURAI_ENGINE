#pragma once




#include <assetManagerWindow/assetManagerWindow.h>
#include <containersWindow/containersWindow.h>
#include <editShortcuts/editShortcuts.h>

#include "input/input.h"
#include "logWindow/logWindow.h"
#include "settings/shortcutApi.h"

namespace samurai
{

	struct Editor
	{

		void init(samurai::ShortcutManager &shortcutManager, samurai::samuraiImgui::ImGuiIdsManager &imguiIDManager);

		void update(const samurai::Input &input, samurai::ShortcutManager &shortcutManager
		            , samurai::LogManager &logs,
		            samurai::LoadedDll &loadedDll,
		            samurai::samuraiImgui::ImGuiIdsManager &imguiIDsManager, samurai::ContainerManager& containerManager);

		void saveFlagsData();

		struct
		{
			bool hideMainWindow = 0;
		}optionsFlags;

		struct
		{
			bool logsWindow = 0;
			bool editShortcutsWindow = 0;
			bool containerManager = 0;
			bool transparentWindow = 0;
			bool assetManagerWindow = 0;
		}windowFlags;

		samurai::LogWindow logWindow;
		samurai::ShortcutsWindow editShortcutsWindow;
		samurai::ContainersWindow containersWindow;
		samurai::AssetManagerWindow assetManagerWindow;

		bool lastHideWindowState = optionsFlags.hideMainWindow;

		bool shouldReloadDll = 0;
		int imguiId = 0;
	};



}
