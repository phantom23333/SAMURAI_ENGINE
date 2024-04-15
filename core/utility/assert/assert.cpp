#include <Config.h>
#include <cstdlib>
#include <cstdio>
#include <assert.h>

#include "settings/log.h"

namespace samurai
{

	namespace assert
	{
		inline void terminate(...)
		{
			std::abort();
		}


	#ifdef SAMURAI_WINDOWS

	#include <Windows.h>

		void assertFunctionDevelopment(const char *expression, const char *file,
			int line, const char *comment)
		{
			char buffer[1024] = {};

			std::snprintf(buffer, sizeof(buffer),
				"Assertion failed\n\n"

				"Expression: \n%s\n\n"

				"File: %s\n"
				"Line: %d\n\n"

				"Comment: \n%s\n\n"

				"Press retry to debug."
				, expression
				, file
				, line
				, comment
			);

			int const action = MessageBoxA(0, buffer, "Pika error", MB_TASKMODAL
				| MB_ICONHAND | MB_ABORTRETRYIGNORE | MB_SETFOREGROUND);

			switch (action)
			{
				case IDABORT: //Abort the program
				{
					terminate();
					return;
				}
				case IDRETRY: //Break execution (debug)
				{
				#ifdef _MSC_VER
					__debugbreak();
				#endif
					terminate();

					return;
				}
				case IDIGNORE: //Ignore assert
				{
					return;
				}
			
			}

		};


		void assertFunctionProduction(const char *expression, const char *file,
			int line, const char *comment)
		{

			char buffer[1024] = {};

			std::snprintf(buffer, sizeof(buffer),
				"Assertion failed\n\n"

				"Expression: \n%s\n\n"

				"File: %s\n"
				"Line: %d\n\n"

				"Comment: \n%s\n\n"

				"Please report this error to the developer."
				, expression
				, file
				, line
				, comment
			);


			int const action = MessageBoxA(0,
				buffer, "Pika error", MB_TASKMODAL
				| MB_ICONHAND | MB_OK | MB_SETFOREGROUND);

			terminate();
		}


		void assertFunctionToLog(const char *expression, const char *file, int line, const char *comment)
		{

			char buffer[1024] = {};

			std::snprintf(buffer, sizeof(buffer),
				"Assertion failed\n"
				"Expression: \n%s\n"
				"File: %s\n"
				"Line: %d\n"
				"Comment: \n%s\n"
				, expression
				, file
				, line
				, comment
			);

			samurai::logToFile(samurai::LogManager::DefaultLogFile, buffer);

		}

	}

#endif


};
