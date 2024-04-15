#pragma once
#include <baseContainer.h>
#include <memory/memoryArena.h>

#include "memory/CustomAllocator.h"

namespace samurai
{


struct RuntimeContainer
{
	//this is the base adress of the runtime container. here is the beginning of all the allocated memory
	void *getBaseAdress() { return arena.containerStructMemory.block; };

	char baseContainerName[50] = {};
	//std::string name = {};

	//this is the pointer to the container virtual class
	Container *pointer = {};

	//this is the container memory arena. here we have all the static data of the container
	samurai::memory::MemoryArena arena = {};

	//this is the allocator of the arena.
	samurai::memory::CustomAllocator allocator = {};
	size_t totalSize = 0;

	//bonus allocators
	samurai::StaticVector<samurai::memory::CustomAllocator, MaxAllocatorsCount> bonusAllocators = {};

	RequestedContainerInfo requestedContainerInfo = {};

	int imguiWindowId = 0;

	unsigned int frameCounter = 0;
	float frameTimer = 0;
	float currentMs = 0;

};


}
