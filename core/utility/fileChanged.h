#pragma once
#include "Config.h"
#include <filesystem>
#include <assert/assert.h>


#ifdef SAMURAI_WINDOWS

#define NOMINMAX
#include <Windows.h>

namespace samurai
{

	
	struct FileChanged
	{
	
		std::filesystem::path path;
		FILETIME time = {};

		void setFile(const char *path);
		bool changed();
	
	private:
	
	};
	
	#endif

};
