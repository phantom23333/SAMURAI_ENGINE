#include "containerManager.h"

#include <unordered_set>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <filesystem>
#include <imgui.h>

#include "imgui_internal.h"
#include "stringManipulation/stringManipulation.h"

samurai::containerId_t samurai::ContainerManager::createContainer(std::string containerName, 
                                                                  samurai::LoadedDll &loadedDll, samurai::LogManager &logManager, 
                                                                  samurai::samuraiImgui::ImGuiIdsManager &imguiIDsManager, std::string &cmd, size_t memoryPos)
{
	
	for(auto &i : loadedDll.containerInfo)
	{
		
		if (i.containerName == containerName)
		{
			return createContainer(i, loadedDll, logManager, imguiIDsManager, cmd, memoryPos);
		}

	}
	
	logManager.log(("Couldn't create container, couldn't find the name: " + containerName).c_str(), samurai::logError);

	return 0;
}


//todo mabe use regions further appart in production
void* samurai::ContainerManager::allocateContainerMemory(samurai::RuntimeContainer &container,
	samurai::ContainerInformation containerInformation, void *memPos)
{
		size_t memoryRequired = containerInformation.calculateMemoryRequirements();

		void *baseMemory = allocateOSMemory(memoryRequired, memPos);

		if (baseMemory == nullptr) { return 0; }

		container.totalSize = memoryRequired;

		allocateContainerMemoryAtBuffer(container, containerInformation, baseMemory);

		return baseMemory;
}

void samurai::ContainerManager::allocateContainerMemoryAtBuffer(samurai::RuntimeContainer &container,
	samurai::ContainerInformation containerInformation, void *buffer)
{
	const size_t staticMemory = containerInformation.containerStructBaseSize;
	const size_t heapMemory = containerInformation.containerStaticInfo.defaultHeapMemorySize;

	char *currentMemoryAdress = (char *)buffer;

	container.arena.containerStructMemory.size = staticMemory;
	container.arena.containerStructMemory.block = currentMemoryAdress;
	currentMemoryAdress += staticMemory;
	samurai::align64(currentMemoryAdress);

	container.allocator.init(currentMemoryAdress, heapMemory);

	currentMemoryAdress += heapMemory;

	for (int i = 0; i < containerInformation.containerStaticInfo.bonusAllocators.size(); i++)
	{
		samurai::align64(currentMemoryAdress);

		samurai::memory::CustomAllocator allocator;
		allocator.init(
			currentMemoryAdress,
			containerInformation.containerStaticInfo.bonusAllocators[i]
		);
		container.bonusAllocators.push_back(allocator);
		currentMemoryAdress += containerInformation.containerStaticInfo.bonusAllocators[i];
	}

}

void samurai::ContainerManager::freeContainerStuff(samurai::RuntimeContainer &container)
{
	deallocateOSMemory(container.arena.containerStructMemory.block);

	//container.arena.dealocateStaticMemory(); //static memory
	//deallocateOSMemory(container.allocator.originalBaseMemory); //heap memory
	//
	//for (auto &i : container.bonusAllocators)
	//{
	//	deallocateOSMemory(i.originalBaseMemory);
	//}
}

samurai::containerId_t samurai::ContainerManager::createContainer
(samurai::ContainerInformation containerInformation,
	samurai::LoadedDll &loadedDll, samurai::LogManager &logManager, samurai::samuraiImgui::ImGuiIdsManager &imguiIDsManager, std::string &cmd,
	size_t memoryPos)
{	
	containerId_t id = ++idCounter;
	
	//not necessary if this is the only things that assigns ids.
	//if (runningContainers.find(id) != runningContainers.end())
	//{
	//	logManager.log((std::string("Container id already exists: #") + std::to_string(id)).c_str(), samurai::logError);
	//	return false;
	//}

	//todo a create and destruct wrapper

	samurai::RuntimeContainer container = {};
	samurai::strlcpy(container.baseContainerName, containerInformation.containerName,
		sizeof(container.baseContainerName));

	if (!allocateContainerMemory(container, containerInformation, (void*)memoryPos))
	{
		logManager.log((std::string("Couldn't allocate memory for constructing container: #") 
			+ std::to_string(id)).c_str(), samurai::logError);
		return 0;
	}

	// In SDL, this coulde be considered to create a texture as render target
	if (containerInformation.containerStaticInfo.requestImguiFbo)
	{
#if !SAMURAI_SDL	
		container.requestedContainerInfo.requestedFBO.createFramebuffer(400, 400); //todo resize small or sthing
#else
		container.requestedContainerInfo.wind = imguiIDsManager.wind;
		if (!container.requestedContainerInfo.wind) {
			SDL_Log("Failed to retrieve window: %s", SDL_GetError());
			// Handle error, possibly with return or exit.
		}
		container.requestedContainerInfo.renderer = SDL_GetRenderer(container.requestedContainerInfo.wind);
		container.requestedContainerInfo.requestedFBO.texture = SDL_CreateTexture(container.requestedContainerInfo.renderer,
						SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 400);
		container.requestedContainerInfo.requestedFBO.width = 400;
		container.requestedContainerInfo.requestedFBO.height = 400;
#endif
		container.imguiWindowId = imguiIDsManager.getImguiIds();
	}

	loadedDll.bindAllocatorDllRealm(&container.allocator);
	
	//this calls the constructors (from the dll realm)
	if (!loadedDll.constructRuntimeContainer(container, containerInformation.containerName.c_str()))
	{
		loadedDll.resetAllocatorDllRealm();

		logManager.log((std::string("Couldn't construct container: #") + std::to_string(id)).c_str(), samurai::logError);

		freeContainerStuff(container);
#if !SAMURAI_SDL
		container.requestedContainerInfo.requestedFBO.deleteFramebuffer();
#else

#endif
		return 0;
	}


	loadedDll.resetAllocatorDllRealm();


#pragma region setup requested container info

	container.requestedContainerInfo.mainAllocator = &container.allocator;
	container.requestedContainerInfo.bonusAllocators = &container.bonusAllocators;
	container.requestedContainerInfo.requestedImguiIds =
		imguiIDsManager.getImguiIds(containerInformation.containerStaticInfo.requestImguiIds);
	container.requestedContainerInfo.imguiTotalRequestedIds = containerInformation.containerStaticInfo.requestImguiIds;

#pragma endregion

	samurai::StaticString<256> cmdArgs = {};
	
	if (cmd.size() > cmdArgs.MAX_SIZE)
	{
		logManager.log(std::string(std::string("Couldn't pass cmd argument because it is too big ")
			+ container.baseContainerName + " #" + std::to_string(id)).c_str(), samurai::logError);
	}
	else
	{
		cmdArgs = cmd.c_str();
	}

	loadedDll.bindAllocatorDllRealm(&container.allocator);
	bool rezult = container.pointer->create(container.requestedContainerInfo, cmdArgs); //this calls create() (from the dll realm)
	loadedDll.resetAllocatorDllRealm();//sets the global allocator back to standard (used for runtime realm)

	runningContainers[id] = container;

	if (!rezult)
	{
		logManager.log((std::string("Couldn't create container because it returned 0")
			+ container.baseContainerName + " #" + std::to_string(id)).c_str(), samurai::logWarning);
		destroyContainer(id, loadedDll, logManager);
		return 0;
	}
	else
	{
		logManager.log(("Created container: " + std::string(container.baseContainerName)).c_str());
	}


	return id;
}

void samurai::ContainerManager::init()
{
}

void samurai::ContainerManager::update(samurai::LoadedDll &loadedDll, samurai::Window &window,
                                    samurai::LogManager &logs, samurai::samuraiImgui::ImGuiIdsManager &imguiIdManager)
{
	SAMURAI_DEVELOPMENT_ONLY_ASSERT(loadedDll.dllHand != 0, "dll not loaded when trying to update containers");

#pragma region reload dll


	//todo try to recover from a failed load

	if (loadedDll.shouldReloadDll())
	{
		reloadDll(loadedDll, window, logs); //todo return 0 on fail

		//todo mark shouldCallReaload or just call reload
		
	}

	
#pragma endregion

	

#pragma region running containers
	for (auto &c : runningContainers)
	{

		{
#if !SAMURAI_SDL			
			SAMURAI_DEVELOPMENT_ONLY_ASSERT(
				(c.second.requestedContainerInfo.requestedFBO.fbo == 0 &&
				c.second.imguiWindowId == 0) ||
				(
				c.second.requestedContainerInfo.requestedFBO.fbo != 0 &&
				c.second.imguiWindowId != 0), "we have a fbo but no imguiwindow id"
			);
#endif
			auto windowInput = window.input;


			auto callUpdate = [&](samurai::WindowState &windowState) -> bool
			{

				auto t1 = std::chrono::high_resolution_clock::now();

				loadedDll.bindAllocatorDllRealm(&c.second.allocator);
				bool rez = c.second.pointer->update(windowInput, windowState, c.second.requestedContainerInfo);
				loadedDll.resetAllocatorDllRealm();

				auto t2 = std::chrono::high_resolution_clock::now();
				
				auto milliseconds = (std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)).count()/1000.f;

				c.second.frameTimer += milliseconds;
				c.second.frameCounter++;
				if (c.second.frameCounter >= 100)
				{
					c.second.currentMs = c.second.frameTimer/100.f;

					c.second.frameTimer = 0;
					c.second.frameCounter = 0;
				}
				
				return rez;

			};

			bool rez = 0;

			if (c.second.imguiWindowId)
			{

			#pragma region imguiwindow
				ImGui::PushID(c.second.imguiWindowId);
				bool isOpen = 1;

				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.0f));
				ImGui::SetNextWindowSize({200,200}, ImGuiCond_Once);
				ImGui::Begin( (std::string("gameplay window id: ") + std::to_string(c.first)).c_str(),
					&isOpen, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				
				//mouse pos and focus
				
				{
					auto windowPos = ImGui::GetWindowPos();

					ImVec2 globalMousePos = {};
					{
						ImGuiContext *g = ImGui::GetCurrentContext();
						globalMousePos = g->IO.MousePos;
					}

					windowInput.mouseX = globalMousePos.x;
					windowInput.mouseY = globalMousePos.y;

					ImVec2 vMin = ImGui::GetWindowContentRegionMin();
					windowInput.mouseX -= windowPos.x + vMin.x;
					windowInput.mouseY -= windowPos.y + vMin.y;


					//https://github.com/ocornut/imgui/issues/5882
					ImGuiViewport *viewPort = ImGui::GetWindowViewport();
					auto io = ImGui::GetIO();
				
					if (viewPort->PlatformUserData)
					{
						windowInput.hasFocus = ImGui::IsWindowFocused()
							&& ImGui::GetPlatformIO().Platform_GetWindowFocus(viewPort) && !io.AppFocusLost;
					}

					//windowInput.hasFocus = windowInput.hasFocus && !io.AppFocusLost;
				}
				

				auto s = ImGui::GetContentRegionMax();
#if !SAMURAI_SDL
				ImGui::Image((void *)c.second.requestedContainerInfo.requestedFBO.texture, s, {0, 1}, {1, 0},
					{1,1,1,1}, {0,0,0,1});
#else
				ImGui::Image((void*)c.second.requestedContainerInfo.requestedFBO.texture, s, { 0, 1 }, { 1, 0 },
					{ 1,1,1,1 }, { 0,0,0,1 });
#endif
				ImGui::End();

				ImGui::PopStyleColor();

				ImGui::PopID();
			#pragma endregion

				auto windowState = window.windowState;
				windowState.w = s.x;
				windowState.h = s.y;

			
#if !SAMURAI_SDL
				c.second.requestedContainerInfo.requestedFBO.resizeFramebuffer(windowState.w, windowState.h);

				glBindFramebuffer(GL_FRAMEBUFFER, c.second.requestedContainerInfo.requestedFBO.fbo);

				rez = callUpdate(windowState);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
				if (c.second.requestedContainerInfo.requestedFBO.width != windowState.w ||
										c.second.requestedContainerInfo.requestedFBO.height != windowState.h)
				{
					c.second.requestedContainerInfo.requestedFBO.width = windowState.w;
					c.second.requestedContainerInfo.requestedFBO.height = windowState.h;
					c.second.requestedContainerInfo.requestedFBO.texture = SDL_CreateTexture(c.second.requestedContainerInfo.renderer,
						SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowState.w, windowState.h);
				}
				SDL_SetRenderTarget(c.second.requestedContainerInfo.renderer, c.second.requestedContainerInfo.requestedFBO.texture);
				rez = callUpdate(windowState);
				
				SDL_SetRenderTarget(c.second.requestedContainerInfo.renderer, nullptr);

#endif
				if (!isOpen)
				{
					rez = 1;
					destroyContainer(c.first, loadedDll, logs);
				}

			}
			else
			{
				rez = callUpdate(window.windowState);
			}

			if (!rez) 
			{
				logs.log(("Terminated container because it returned 0: " + std::string(c.second.baseContainerName)
					+ " #" + std::to_string(c.first)).c_str());
				destroyContainer(c.first, loadedDll, logs);
			}

		}

	}
#pragma endregion

}

void samurai::ContainerManager::reloadDll(samurai::LoadedDll &loadedDll, samurai::Window &window, samurai::LogManager &logs)
{

	std::this_thread::sleep_for(std::chrono::milliseconds(200));


	auto oldContainerInfo = loadedDll.containerInfo;

	if (!loadedDll.tryToloadDllUntillPossible(loadedDll.id, logs, std::chrono::seconds(5)))
	{
		logs.log("Couldn't reloaded dll", samurai::logWarning);
		return;
	}
	//todo pospone dll reloading

	std::unordered_map<std::string, samurai::ContainerInformation> containerNames;
	for (auto &c : loadedDll.containerInfo)
	{
		containerNames[c.containerName] = c;
	}

	std::unordered_map<std::string, samurai::ContainerInformation> oldContainerNames;
	for (auto &c : oldContainerInfo)
	{
		oldContainerNames[c.containerName] = c;
	}

	//clear containers that dissapeared
	{


		std::vector<samurai::containerId_t> containersToClean;
		for (auto &i : runningContainers)
		{
			if (containerNames.find(i.second.baseContainerName) ==
				containerNames.end())
			{
				std::string l = "Killed container because it does not exist anymore in dll: " + 
					std::string(i.second.baseContainerName)
					+ " #" + std::to_string(i.first);
				logs.log(l.c_str(), samurai::logError);

				containersToClean.push_back(i.first);
			}
		}

		for (auto i : containersToClean)
		{
			forceTerminateContainer(i, loadedDll, logs);
		}
	}

	//clear containers that changed static info
	{

		std::vector<samurai::containerId_t> containersToClean;
		for (auto &i : runningContainers)
		{

			auto &newContainer = containerNames[i.second.baseContainerName];
			auto &oldContainer = oldContainerNames[i.second.baseContainerName];

			if (newContainer != oldContainer)
			{
				std::string l = "Killed container because its static container info\nhas changed: "
					+ std::string(i.second.baseContainerName)
					+ " #" + std::to_string(i.first);
				logs.log(l.c_str(), samurai::logError);

				containersToClean.push_back(i.first);
			}

		}

		for (auto i : containersToClean)
		{
			forceTerminateContainer(i, loadedDll, logs);
		}

	}

	//realocate pointers
	{
		std::unordered_map<std::string, size_t> vtable;

		for (auto& i : runningContainers)
		{
			auto pos = vtable.find(i.second.baseContainerName);
			if (pos == vtable.end())
			{
				samurai::RuntimeContainer container = {};
				samurai::strlcpy(container.baseContainerName, i.second.baseContainerName,
					sizeof(container.baseContainerName));

				samurai::ContainerInformation info;
				for (auto& l : loadedDll.containerInfo)
				{
					if (l.containerName == i.second.baseContainerName)
					{
						info = l;
					}
				}

				if (!allocateContainerMemory(container, info, 0))
				{
					logs.log("Internal error 1 (couldn't allocate container memory)", samurai::logError);
				}
				else
				{
					loadedDll.bindAllocatorDllRealm(&container.allocator);

					//this calls the constructors (from the dll realm)
					if (!loadedDll.constructRuntimeContainer(container, i.second.baseContainerName))
					{
						loadedDll.resetAllocatorDllRealm();
						logs.log("Internal error 2", samurai::logError);
						freeContainerStuff(container);
						loadedDll.resetAllocatorDllRealm();
					}
					else
					{

						size_t id = *(size_t*)container.pointer;

						freeContainerStuff(container);
						loadedDll.resetAllocatorDllRealm();

						vtable[i.second.baseContainerName] = id;
					}


				}
			}

			pos = vtable.find(i.second.baseContainerName);
			if (pos != vtable.end())
			{
				memcpy(i.second.pointer, (void*)&pos->second, sizeof(size_t));
			}
		}
	}



	loadedDll.gameplayReload_(window.context);
	


	logs.log("Reloaded dll");

}

//not verbose flag
bool samurai::ContainerManager::destroyContainer(containerId_t id, samurai::LoadedDll &loadedDll,
	samurai::LogManager &logManager)
{
	SAMURAI_DEVELOPMENT_ONLY_ASSERT(loadedDll.dllHand != 0, "dll not loaded when trying to destroy container");

	auto c = runningContainers.find(id);
	if (c == runningContainers.end())
	{
		logManager.log((std::string("Couldn't find container for destruction: #") + std::to_string(id)).c_str(),
			samurai::logError);
		return false;
	}

	auto name = c->second.baseContainerName;

	loadedDll.bindAllocatorDllRealm(&c->second.allocator);
	loadedDll.destructContainer_(&(c->second.pointer), &c->second.arena);
	loadedDll.resetAllocatorDllRealm();

	freeContainerStuff(c->second);

#if !SAMURAI_SDL
	c->second.requestedContainerInfo.requestedFBO.deleteFramebuffer();
#else
	SDL_DestroyTexture(c->second.requestedContainerInfo.requestedFBO.texture);
#endif
	runningContainers.erase(c);

	logManager.log(std::string("Destroyed continer").c_str());

	return true;
}

//todo remove some of this functions in production



bool samurai::ContainerManager::forceTerminateContainer(containerId_t id, samurai::LoadedDll &loadedDll, samurai::LogManager &logManager)
{
	SAMURAI_DEVELOPMENT_ONLY_ASSERT(loadedDll.dllHand != 0, "dll not loaded when trying to destroy container");

	auto c = runningContainers.find(id);
	if (c == runningContainers.end())
	{
		logManager.log((std::string("Couldn't find container for destruction: #") + std::to_string(id)).c_str(),
			samurai::logError);
		return false;
	}

	auto name = c->second.baseContainerName;

	freeContainerStuff(c->second);

	runningContainers.erase(c);

	logManager.log((std::string("Force terminated continer: ") + name + " #" + std::to_string(id)).c_str());
#if !SAMURAI_SDL
	c->second.requestedContainerInfo.requestedFBO.deleteFramebuffer();
#else
	SDL_DestroyTexture(c->second.requestedContainerInfo.requestedFBO.texture);
#endif
	return true;
}

void samurai::ContainerManager::destroyAllContainers(samurai::LoadedDll &loadedDll,
	samurai::LogManager &logManager)
{
	std::vector < samurai::containerId_t> containersId;
	containersId.reserve(runningContainers.size());

	for (auto &c : runningContainers)
	{
		containersId.push_back(c.first);
	}

	for (auto i : containersId)
	{
		destroyContainer(i, loadedDll, logManager);
	}

}

#ifdef PIKA_PRODUCTION

void *samurai::ContainerManager::allocateOSMemory(size_t size, void *baseAdress)
{
	SAMURAI_ASSERT(baseAdress == nullptr, "can't allocate fixed memory in production");
	return malloc(size);
}

void samurai::ContainerManager::deallocateOSMemory(void *baseAdress)
{
	free(baseAdress);
}

#else

#include <Windows.h>

void *samurai::ContainerManager::allocateOSMemory(size_t size, void *baseAdress)
{
	return VirtualAlloc(baseAdress, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void samurai::ContainerManager::deallocateOSMemory(void *baseAdress)
{
	VirtualFree(baseAdress, 0, MEM_RELEASE);
}



#endif