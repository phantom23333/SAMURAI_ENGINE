#pragma once

#include <Config.h>

#include <containerManager/containerManager.h>
#include <dllLoader/dllLoader.h>
#include <IconsForkAwesome.h>
#include <imgui.h>

#include "settings/log.h"
#include "windowGraphic/gui.h"

namespace samurai
{

	struct ContainersWindow
	{

		void init(samurai::samuraiImgui::ImGuiIdsManager &imguiIdsManager);

		void update(samurai::LogManager& logManager, bool& open, samurai::LoadedDll& loadedDll,
		            samurai::ContainerManager& containerManager, samurai::samuraiImgui::ImGuiIdsManager& imguiIdsManager);

		static constexpr char *ICON = ICON_FK_MICROCHIP;
		static constexpr char *NAME = "Containers manager";
		static constexpr char *ICON_NAME = ICON_FK_MICROCHIP " Containers manager";

		char filterContainerInfo[50] = {};
		char filterSnapshots[50] = {};
		char snapshotName[50] = {};
		char recordingName[50] = {};

		int itemCurrentAvailableCOntainers = 0;
		int itemCurrentCreatedContainers = 0;
		int currentSelectedSnapshot = 0;
		int currentSelectedRecording = 0;

		bool createAtSpecificMemoryRegion = 0;

		int imguiIds = 0;
	};



}
