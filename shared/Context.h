#pragma once

#include <SDL.h>

#include "memory/CustomAllocator.h"

struct GLFWwindow;
struct ImGuiContext;

namespace samurai
{
	struct Context
	{
		ImGuiContext* ImGuiContext = {};
#if !SAMURAI_SDL
		using glfwMakeContextCurrent_t = void(GLFWwindow *);

		glfwMakeContextCurrent_t *glfwMakeContextCurrentPtr = {};
		GLFWwindow *wind = {};
#else
		SDL_Window *wind = {};
		SDL_Renderer *renderer = {};

#endif
		samurai::memory::CustomAllocator imguiAllocator;
	};
};