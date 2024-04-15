#pragma once

#include <IconsForkAwesome.h>
#include <imgui.h>
#include "settings/log.h"
#include "windowGraphic/gui.h"


namespace samurai
{


	struct LogWindow
	{

		void init(samurai::samuraiImgui::ImGuiIdsManager &idManager);

		void update(samurai::LogManager &logManager, bool &open);

		static constexpr char *ICON = ICON_FK_COMMENT_O;
		static constexpr char *NAME = "logs";
		static constexpr char *ICON_NAME = ICON_FK_COMMENT_O " logs";
		bool autoScroll = true;
		ImGuiTextFilter filter;

		int imguiId = 0;
	};

}
