#pragma once

#include <string>
#include <unordered_map>

#include "runtimeContainer.h"
#include "settings/log.h"
#include "windowGraphic/gui.h"
#include "windowGraphic/window.h"
#include <dllLoader/dllLoader.h>

namespace samurai
{
	using containerId_t = unsigned int;
	
	struct ContainerManager
	{

		std::unordered_map<containerId_t, samurai::RuntimeContainer> runningContainers;
		
		containerId_t createContainer(
			samurai::ContainerInformation containerInformation,
			samurai::LoadedDll& loadedDll, samurai::LogManager& logManager,
			samurai::samuraiImgui::ImGuiIdsManager& imguiIDsManager, std::string& cmd, size_t memoryPos = 0);

		containerId_t createContainer(
			std::string containerName,
			samurai::LoadedDll& loadedDll, samurai::LogManager& logManager,
			samurai::samuraiImgui::ImGuiIdsManager& imguiIDsManager, std::string& cmd, size_t memoryPos = 0);

		void* allocateContainerMemory(samurai::RuntimeContainer &container, samurai::ContainerInformation containerInformation, void *memPos = 0);

		//buffer should have the correct size
		void allocateContainerMemoryAtBuffer(samurai::RuntimeContainer &container,
			samurai::ContainerInformation containerInformation, void *buffer);


		//deallocates memory, does not call destructors
		void freeContainerStuff(samurai::RuntimeContainer &container);

		void init();

		void update(
			samurai::LoadedDll &loadedDll,
			samurai::Window &window,
			samurai::LogManager &logs,
			samurai::samuraiImgui::ImGuiIdsManager &imguiIdManager);

		void reloadDll(samurai::LoadedDll &loadedDll,
			samurai::Window &window,
			samurai::LogManager &logs);

		bool destroyContainer(containerId_t id, samurai::LoadedDll &loadedDll,
			samurai::LogManager &logManager);

		//same as destroy container but doesn't call user destructors
		bool forceTerminateContainer(containerId_t id, samurai::LoadedDll &loadedDll,
			samurai::LogManager &logManager);

		void destroyAllContainers(samurai::LoadedDll &loadedDll,
			samurai::LogManager &logManager);

		containerId_t idCounter = 0;

		//todo move outside
		void *allocateOSMemory(size_t size, void* baseAdress = 0);

		void deallocateOSMemory(void *baseAdress);
	};


}
