#pragma once

#include <Config.h>
#include <baseContainer.h>
//this is used to declare containers
#include "memory/memoryArena.h"


Container *getContainer(const char* name, samurai::memory::MemoryArena *memoryArena);


#include "containers/pikaGameplay.h"
#include "containers/mario/mario.h"
#include "containers/mario/marioEditor.h"

#define SAMURAI_ALL_CONTAINERS() \
	SAMURAI_DECLARE_CONTAINER(Gameplay) \
	SAMURAI_DECLARE_CONTAINER(Mario) \
	SAMURAI_DECLARE_CONTAINER(MarioEditor)
