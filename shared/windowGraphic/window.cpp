#include "window.h"

#ifdef SAMURAI_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <Windows.h>
#include <GLFW/glfw3native.h>
#endif





struct WindowRect
{
	int x = 100, y = 100, z = 640, w = 480;
};

void samurai::Window::create()
{

	WindowRect wr = {};

#if PIKA_DEVELOPMENT
	if (sfs::safeLoad(&wr, sizeof(wr), PIKA_ENGINE_SAVES_PATH "windowPos", false) != sfs::noError)
	{
		wr = {};
	}
#endif
	
	if (wr.x < 0 || wr.y < 0 || wr.z <= 0 || wr.w <= 0)
	{
		wr = {};
	}
#if !SAMURAI_SDL	
	//todo debug from engine
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	//glfwWindowHint(GLFW_SAMPLES, 1);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	context.wind = glfwCreateWindow(wr.z, wr.w, "SamuraiPika", NULL, NULL);
	glfwSetWindowPos(context.wind, wr.x, wr.y);

	input.hasFocus = true;

	SAMURAI_ASSERT(context.wind, "problem initializing window");
	glfwMakeContextCurrent(context.wind);

	glfwSetWindowUserPointer(context.wind, this);

	glfwSetMouseButtonCallback(context.wind, mouseCallback);
	glfwSetWindowFocusCallback(context.wind, windowFocusCallback);
	glfwSetCharCallback(context.wind, characterCallback);
	glfwSetKeyCallback(context.wind, keyCallback);
#else

	context.wind = SDL_CreateWindow("SamuraiPika", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wr.z, wr.w, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	SAMURAI_ASSERT(context.wind, "problem initializing window");
	context.renderer = SDL_CreateRenderer(context.wind, -1, SDL_RENDERER_TARGETTEXTURE);
	input.hasFocus = true;
#endif
	// allocate imgui memory with separate allocator
	context.imguiAllocator.init(malloc(samurai::MB(20)), samurai::MB(20));

	


	timer = std::chrono::high_resolution_clock::now();
}

void samurai::Window::saveWindowPositions()
{


	WindowRect wr = {};
#if !SAMURAI_SDL
	glfwGetWindowPos(context.wind, &wr.x, &wr.y);
#else
	SDL_GetWindowPosition(context.wind, &wr.x, &wr.y);
#endif

	wr.z = windowState.w;
	wr.w = windowState.h;

	sfs::safeSave(&wr, sizeof(wr), SAMURAI_ENGINE_SAVES_PATH "windowPos", false);



}

bool samurai::Window::shouldClose()
{
#if !SAMURAI_SDL
	return glfwWindowShouldClose(context.wind);
#else
	return SDL_QuitRequested();
#endif
}

void samurai::Window::update()
{
#pragma region deltaTime
	auto end = std::chrono::high_resolution_clock::now();
	input.deltaTime = (std::chrono::duration_cast<std::chrono::microseconds>(end - timer)).count() / 1000000.0f;
	timer = end;

	if (input.deltaTime > 1.f / 10) { input.deltaTime = 1.f / 10; }
#pragma endregion

#pragma region input

	auto processInputBefore = [](samurai::Button &b)
	{
		b.setTyped(false);
	};

	processInputBefore(input.lMouse);
	processInputBefore(input.rMouse);

	for (int i = 0; i < Button::BUTTONS_COUNT; i++)
	{
		processInputBefore(input.buttons[i]);
	}

	memset(input.typedInput, 0, sizeof(input.typedInput));

#pragma endregion

#if !SAMURAI_SDL
	glfwPollEvents();
	glfwSwapBuffers(context.wind);
#else
	input.listen();
	SDL_RenderPresent(context.renderer);
#endif


#pragma region window state

	{
		int w = 0;
		int h = 0;
#if !SAMURAI_SDL
		glfwGetWindowSize(context.wind, &w, &h);
#else
		SDL_GetWindowSize(context.wind, &w, &h);
#endif
		windowState.w = w;
		windowState.h = h;

	}

#pragma endregion


#pragma region input

	double mouseX = 0;
	double mouseY = 0;
#if !SAMURAI_SDL
	glfwGetCursorPos(context.wind, &mouseX, &mouseY);
#else
	SDL_GetMouseState((int*)&mouseX, (int*)&mouseY);
#endif
	input.mouseX = (int)mouseX;
	input.mouseY = (int)mouseY;

	auto processInput = [](samurai::Button &b)
	{

		if (!b.lastState() && b.held())
		{
			b.setPressed(true);
			b.setTyped(true);
		}
		else
		{
			b.setPressed(false);
		}

		if (b.lastState() && !b.held())
		{
			b.setReleased(true);
		}
		else
		{
			b.setReleased(false);
		}

		b.setLastState(b.held());

	};

	processInput(input.lMouse);
	processInput(input.rMouse);

	for (int i = 0; i < Button::BUTTONS_COUNT; i++)
	{
		processInput(input.buttons[i]);
	}


#pragma endregion

}
