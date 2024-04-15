
#include <filesystem>


#include "dllLoader/dllLoader.h"


#include <editor/editor.h>

#include <containerManager/containerManager.h>



static bool shouldClose = false;

int main(int argc, char** argv)
{


#pragma region init global variables stuff
	samurai::initShortcutApi();
#pragma endregion

#pragma region log
	samurai::LogManager logs;
	logs.init(samurai::LogManager::DefaultLogFile);

#pragma endregion


#pragma region load dll
	std::filesystem::path currentPath = std::filesystem::current_path();
	samurai::LoadedDll loadedDll;
	SAMURAI_ASSERT(loadedDll.tryToloadDllUntillPossible(0, logs, std::chrono::seconds(5)),
		"Couldn't load dll");
#pragma endregion
	
#pragma region pika imgui id manager
	samurai::samuraiImgui::ImGuiIdsManager imguiIdsManager;
#pragma endregion

#pragma region push notification manager

#pragma endregion


#pragma region init window opengl imgui and context
#if !SAMURAI_SDL
	SAMURAI_ASSERT(glfwInit(), "Problem initializing glfw");
	//glfwSetErrorCallback(error_callback); todo
	samurai::Window window = {};
	window.create();

	SAMURAI_ASSERT(gladLoadGL(), "Problem initializing glad");

	//logs.log((const char*)glGetString(GL_VERSION));


	samurai::samuraiImgui::initImgui(window.context);

	window.context.glfwMakeContextCurrentPtr = glfwMakeContextCurrent;
#else
	SDL_Init(SDL_INIT_EVERYTHING);
	samurai::Window window = {};
	window.create();
	samurai::samuraiImgui::initImgui(window.context);
	imguiIdsManager.wind = window.context.wind;
#endif
#pragma endregion

#pragma region container manager

	samurai::ContainerManager containerManager;

	containerManager.init();

#pragma endregion

#pragma region init dll reaml

	loadedDll.gameplayStart_(window.context);

	
#pragma endregion



#pragma region shortcuts
	samurai::ShortcutManager shortcutManager;
#pragma endregion

#pragma region editor

	samurai::Editor editor; 
	

	editor.init(shortcutManager, imguiIdsManager);

#pragma endregion

	auto container = containerManager.createContainer
	(loadedDll.containerInfo[0], loadedDll, logs, imguiIdsManager, std::string());


	while (!shouldClose)
	{
		if (window.shouldClose())
		{
			shouldClose = true;
			break;
		}


	#pragma region start imgui
		samurai::samuraiImgui::imguiStartFrame(window.context);
	#pragma endregion

	#pragma region clear screen


#if !SAMURAI_SDL
		glClear(GL_COLOR_BUFFER_BIT);
#else
		SDL_SetRenderDrawColor(window.context.renderer, 0, 0, 0, 255);
		SDL_RenderClear(window.context.renderer);
#endif


	#pragma endregion

	#pragma region editor stuff
	editor.update(window.input, shortcutManager, logs, 
		              loadedDll, imguiIdsManager, containerManager);
	#pragma endregion



	#pragma region container manager

	if (editor.shouldReloadDll)
	{
		editor.shouldReloadDll = false;
		containerManager.reloadDll(loadedDll, window, logs);
	}

	containerManager.update(loadedDll, window, logs, imguiIdsManager);

	#pragma endregion

	#pragma region end imgui frame
		samurai::samuraiImgui::imguiEndFrame(window.context);
	#pragma endregion

	#pragma region window update
		window.update();
	#pragma endregion

	#pragma region shortcut manager update
		shortcutManager.update(window.input);
	#pragma endregion

	editor.saveFlagsData();


	window.saveWindowPositions();

	}

	editor.saveFlagsData();

	window.saveWindowPositions();


	containerManager.destroyAllContainers(loadedDll, logs);

	
	

	//terminate();

	return 0;
}