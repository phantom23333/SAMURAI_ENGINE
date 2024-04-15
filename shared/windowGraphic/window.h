#pragma once
#include <Context.h>
#include <chrono>

#include "input/input.h"
#include <safeSave/safeSave.h>

#include "assert/assert.h"
#include "input/callbacks.h"
namespace samurai
{

	
	struct WindowState
	{
		int w = 0;
		int h = 0;


	};
	

	//this is not intended to have multiple instances in the program
	struct Window
	{
		samurai::Context context = {};


		//this is made to be passed to the user code
		//on live code editing this will be recorded every frame
		Input input = {};


		WindowState windowState = {};

		//this doesn't return error codes because it will do the asserts for you
		void create();

		void saveWindowPositions();

		bool shouldClose();

		void update();

		std::chrono::steady_clock::time_point timer = {};
	};



}
