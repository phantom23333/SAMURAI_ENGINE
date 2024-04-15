#include "dllMain.h"



#include "containers/pikaGameplay.h"
#include <containers.h>

#include "windowGraphic/gui.h"

//todo use a global static array that can be accessed from other cpps and the macro will create an instance of a struct 
//that will push that container
#define SAMURAI_DECLARE_CONTAINER(x) info.push_back( samurai::ContainerInformation(sizeof(x), #x, x::containerInfo()) );

CORE_API void getContainersInfo(std::vector<samurai::ContainerInformation> &info)
{
	info.clear();
	SAMURAI_ALL_CONTAINERS()

}

#undef SAMURAI_DECLARE_CONTAINER

//this should not allocate memory
CORE_API bool constructContainer(Container **c, samurai::memory::MemoryArena *arena, const char *name)
{
	*c = getContainer(name, arena);
	return *c != 0;

}

CORE_API void destructContainer(Container **c, samurai::memory::MemoryArena *arena)
{
	//no need to call delete.
	(*c)->~Container();

}

CORE_API void bindAllocator(samurai::memory::CustomAllocator *arena)
{
	samurai::memory::setGlobalAllocator(arena);
}

CORE_API void resetAllocator()
{
	samurai::memory::setGlobalAllocatorToStandard();
}

CORE_API void dissableAllocators()
{
	samurai::memory::dissableAllocators();
}

//used to initialize libraries 
CORE_API void gameplayStart(samurai::Context &context)
{
	samurai::samuraiImgui::setImguiAllocator(context.imguiAllocator);

	
#pragma region init stuff
#if !SAMURAI_SDL
	SAMURAI_ASSERT(gladLoadGL(), "Problem initializing glad from dll");
	samurai::samuraiImgui::setImguiContext(context);
	gl2d::init();
#else
	// TODO: SDL stuff here
	samurai::samuraiImgui::setImguiContext(context);

#endif

#pragma endregion
}


//this won't be ever called in production so we can remove the code
CORE_API void gameplayReload(samurai::Context &context)
{

	samurai::samuraiImgui::setImguiAllocator(context.imguiAllocator); 
	//samurai::initShortcutApi();

#if !SAMURAI_SDL
	SAMURAI_ASSERT(gladLoadGL(), "Problem initializing glad from dll");
	samurai::samuraiImgui::setImguiContext(context);

	gl2d::init();
#else
	samurai::samuraiImgui::setImguiContext(context);
#endif


}

#if SAMURAI_WINDOWS
#ifdef SAMURAI_DEVELOPMENT


#include <Windows.h>
//https://learn.microsoft.com/en-us/windows/win32/dlls/dllmain
BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpvReserved)  // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	// Initialize once for each new process.
	// Return FALSE to fail DLL load.
	break;

	case DLL_THREAD_ATTACH:
	// Do thread-specific initialization.
	break;

	case DLL_THREAD_DETACH:
	// Do thread-specific cleanup.
	break;

	case DLL_PROCESS_DETACH:

	if (lpvReserved != nullptr)
	{
		break; // do not do cleanup if process termination scenario
	}

	//if (old)
	//{
	//	std::cout.rdbuf(old);
	//}

	// Perform any necessary cleanup.
	break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}


#endif
#endif
