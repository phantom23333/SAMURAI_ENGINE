#pragma once

#include <Config.h>
#include <gl2d/gl2d.h>
#include <imgui.h>
#include <containers.h>
#include <containerInformation.h>
#include <vector>
#include <sstream>

#include "memory/memoryArena.h"

CORE_API void gameplayStart(samurai::Context &context);
CORE_API void setConsoleBuffer(std::streambuf *buf);
CORE_API void gameplayReload(samurai::Context &context);
CORE_API void getContainersInfo(std::vector<samurai::ContainerInformation> &info);
CORE_API bool constructContainer(Container **c, samurai::memory::MemoryArena *arena, const char *name);
CORE_API void destructContainer(Container **c, samurai::memory::MemoryArena *arena);
CORE_API void bindAllocator(samurai::memory::CustomAllocator *arena);
CORE_API void resetAllocator();
CORE_API void dissableAllocators();
