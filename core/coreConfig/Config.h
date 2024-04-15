#pragma once





#define SAMURAI_INTERNAL_CURRENT_ASSERT_FUNCTION samurai::assert::assertFunctionDevelopment



#define SAMURAI_ENGINE_SAVES_PATH SAMURAI_ENGINE_RESOURCES_PATH "/engineSaves/"



#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)

#define SAMURAI_WINDOWS 1

#elif defined(__linux__)

#define SAMURAI_LINUX 1

#else

#error "supports only windows"

#endif

#pragma endregion

#ifdef SAMURAI_DEVELOPMENT
#define CORE_API extern "C" __declspec(dllexport)
#endif


#define SAMURAI_SDL 1