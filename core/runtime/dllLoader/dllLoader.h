#pragma once
#include <baseContainer.h>
#include <containerInformation.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "memory/memoryArena.h"
#include "runtimeContainer.h"
#include "settings/log.h"

#define GAMEPLAYSTART(x) void x(samurai::Context &context)
typedef GAMEPLAYSTART(gameplayStart_t);
#undef GAMEPLAYSTART

#define GAMEPLAYRELOAD(x) void x(samurai::Context &context)
typedef GAMEPLAYRELOAD(gameplayReload_t);
#undef GAMEPLAYRELOAD

#define GETCONTAINERSINFO(x) void x(std::vector<samurai::ContainerInformation> &info)
typedef GETCONTAINERSINFO(getContainersInfo_t);
#undef GETCONTAINERSINFO

#define CONSTRUCTCONTAINER(x) bool x(Container **c, samurai::memory::MemoryArena *arena, const char *name);
typedef CONSTRUCTCONTAINER(constructContainer_t);
#undef CONSTRUCTCONTAINER

#define DESTRUCTCONTAINER(x) void x(Container **c, samurai::memory::MemoryArena *arena);
typedef DESTRUCTCONTAINER(destructContainer_t);
#undef DESTRUCTCONTAINER

#define BINDALLOCATOR(x) void x(samurai::memory::CustomAllocator *arena);
typedef BINDALLOCATOR(bindAllocator_t);
#undef BINDALLOCATOR

#define RESETALLOCATOR(x) void x();
typedef RESETALLOCATOR(resetAllocator_t)
#undef RESETALLOCATOR

#define DISSABLEALLOCATORS(x) void x();
typedef DISSABLEALLOCATORS(dissableAllocators_t)
#undef DISSABLEALLOCATORS




#ifdef SAMURAI_WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif

namespace samurai
{


struct LoadedDll
{
	gameplayStart_t *gameplayStart_ = {};
	gameplayReload_t *gameplayReload_ = {};
	getContainersInfo_t *getContainersInfo_ = {};
	constructContainer_t *constructContainer_ = {};
	destructContainer_t *destructContainer_ = {};
	bindAllocator_t *bindAllocator_ = {};
	resetAllocator_t *resetAllocator_ = {};
	dissableAllocators_t *dissableAllocators_ = {};

#ifdef SAMURAI_WINDOWS
	FILETIME filetime = {};
	HMODULE dllHand = {};
#endif

	int id = 0;

	bool loadDll(int id, samurai::LogManager &logs);

	bool tryToloadDllUntillPossible(int id, samurai::LogManager &logs, std::chrono::duration<long long> timeout = 
		std::chrono::seconds(0));

	void unloadDll();

	//no need to call since it is called in load dll function
	void getContainerInfoAndCheck(samurai::LogManager &logs);

	bool shouldReloadDll();

	void reloadContainerExtensionsSupport();

	std::vector<samurai::ContainerInformation> containerInfo;

	std::unordered_map<std::string, std::string> containerExtensionsSupport;

	bool constructRuntimeContainer(samurai::RuntimeContainer &c, const char *name);

	void bindAllocatorDllRealm(samurai::memory::CustomAllocator *allocator);

	void resetAllocatorDllRealm();

	bool checkIfDllIsOpenable();
};



};
