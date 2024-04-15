#pragma once



#include <IconsForkAwesome.h>

#include "settings/shortcutApi.h"
#include "windowGraphic/gui.h"

namespace samurai
{

	struct ShortcutsWindow
	{


		void init(samurai::samuraiImgui::ImGuiIdsManager &imguiIdManager);

		void update(samurai::ShortcutManager &shortcutManager, bool &open);

		static constexpr char *ICON = ICON_FK_PENCIL_SQUARE;
		static constexpr char *NAME = "Edit Shortcuts...";
		static constexpr char *ICON_NAME = ICON_FK_PENCIL_SQUARE " Edit Shortcuts...";

		int imguiId = 0;
	};



};
