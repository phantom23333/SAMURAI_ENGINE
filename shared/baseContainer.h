#pragma once
#include <iostream>
#include <string>
#include <staticVector.h>
#include <fstream>
#include <staticString.h>
#include <SDL.h>

#include "input/input.h"
#include "memory/CustomAllocator.h"
#include "memory/globalAllocator.h"
#include "openGL/frameBuffer.h"
#include "windowGraphic/window.h"

#define READENTIREFILE(x) bool x(const char* name, void* buffer, size_t size)
typedef READENTIREFILE(readEntireFile_t);
#undef READENTIREFILE

#define GETFILESIZE(x) bool x(const char* name, size_t &size)
typedef GETFILESIZE(getFileSize_t);
#undef GETFILESIZE

static constexpr size_t MaxAllocatorsCount = 128;

#if SAMURAI_SDL
	struct PikaFramebuffer{
		SDL_Texture* texture = {};
		int width = 0;
		int height = 0;
	};
#endif

//this is passed by the engine. You should not modify the data
//this is also used by the engine to give you acces to some io functions
struct RequestedContainerInfo
{
	samurai::memory::CustomAllocator *mainAllocator = {};
	samurai::StaticVector<samurai::memory::CustomAllocator, MaxAllocatorsCount> *bonusAllocators = {};

	//readEntireFile_t *readEntireFilePointer = {};
	//getFileSize_t *getFileSizePointer = {};
#if !SAMURAI_SDL
	samurai::GL::PikaFramebuffer requestedFBO = {};
#else
	SDL_Window* wind = {};
	SDL_Renderer* renderer = {};
	PikaFramebuffer requestedFBO = {};
#endif

	int requestedImguiIds = 0;
	int imguiTotalRequestedIds = 0;


	bool readEntireFileBinary(const char *name, void *buffer, size_t size)
	{
		//PIKA_DEVELOPMENT_ONLY_ASSERT(readEntireFilePointer, "read entire file pointer not assigned");
		bool success = true;

		samurai::memory::setGlobalAllocatorToStandard();
		{
			std::ifstream f(name, std::ios::binary);

			if (!f.is_open())
			{
				success = false;
			}
			else
			{
				f.read((char*)buffer, size);
				f.close();
			}
		}
		samurai::memory::setGlobalAllocator(mainAllocator);

		return success;
	}

	bool getFileSizeBinary(const char *name, size_t &size)
	{
		//PIKA_DEVELOPMENT_ONLY_ASSERT(getFileSizePointer, "get file size pointer not assigned");

		bool success = true;
		size = 0;

		//todo push pop allocator or use that pointer thing (and don't forget to only use explicit allocators calls or sthing)
		samurai::memory::setGlobalAllocatorToStandard();
		{
			std::ifstream f(name, std::ifstream::ate | std::ifstream::binary);
			if (!f.is_open())
			{
				success = false;
			}
			else
			{
				size = f.tellg();
				f.close();
			}
		}
		samurai::memory::setGlobalAllocator(mainAllocator);

		return size;
	}

};


struct ContainerStaticInfo
{

	//this is the main heap allocator memory size
	size_t defaultHeapMemorySize = 0;
	

	//this will use the global allocator. you won't be able to use input recording or snapshots, and the 
	//memory leak protection won't be possible.
	bool useDefaultAllocator = 0;


	samurai::StaticVector<size_t, MaxAllocatorsCount> bonusAllocators = {};

	//add file extensions here so that the engine knows that your container can open them.
	samurai::StaticVector<samurai::StaticString<16>, 16> extensionsSuported = {};

	//the engine will create a new window for your container and give you the fbo to bind to
	//in release that fbo will just be the default framebuffer
	bool requestImguiFbo = 0;

	unsigned int requestImguiIds = 0;

	bool _internalNotImplemented = 0;

	bool operator==(const ContainerStaticInfo &other)
	{
		if (this == &other) { return true; }

		return
			this->defaultHeapMemorySize == other.defaultHeapMemorySize &&
			this->bonusAllocators == other.bonusAllocators &&
			this->_internalNotImplemented == other._internalNotImplemented &&
			this->requestImguiFbo == other.requestImguiFbo &&
			this->requestImguiIds == other.requestImguiIds &&
			this->useDefaultAllocator == other.useDefaultAllocator;
		;
	}

	bool operator!=(const ContainerStaticInfo &other)
	{
		return !(*this == other);
	}


};


struct Container
{

	//this is used to give to the engine basic information about your container.
	//this function should be pure
	//this function should not allocate memory
	//this should not be dependent on anything that is called on create or library initialization
	static ContainerStaticInfo containerInfo() { ContainerStaticInfo c; c._internalNotImplemented = true; return c; };
	

	virtual bool create(RequestedContainerInfo &requestedInfo, samurai::StaticString<256> commandLineArgument) = 0;

	virtual bool update(
		samurai::Input input, 
		samurai::WindowState windowState,
		RequestedContainerInfo &requestedInfo) = 0;

	virtual ~Container() {};

};